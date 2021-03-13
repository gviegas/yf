//
// CG
// PassVK.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <stdexcept>

#include "PassVK.h"
#include "ImageVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

// ------------------------------------------------------------------------
// PassVK

PassVK::PassVK(const vector<ColorAttach>* colors,
               const vector<ColorAttach>* resolves,
               const DepStenAttach* depthStencil)
  : Pass(colors, resolves, depthStencil) {

  vector<VkAttachmentDescription> attachDescs;
  vector<VkAttachmentReference> attachRefs;

  // Define attachments
  if (colors) {
    auto add = [&](const vector<ColorAttach>& attachs) {
      for (const auto& attach : attachs) {
        if (aspectOfVK(attach.format) != VK_IMAGE_ASPECT_COLOR_BIT)
          throw invalid_argument("Invalid format for color attachment");

        attachDescs.push_back({});

        auto& desc = attachDescs.back();
        desc.flags = 0;
        desc.format = toFormatVK(attach.format);
        desc.samples = toSampleCountVK(attach.samples);
        desc.loadOp = toLoadOpVK(attach.loadOp);
        desc.storeOp = toStoreOpVK(attach.storeOp);
        desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        // XXX: won't work if an image view is used in more than one pass
        desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        desc.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

        attachRefs.push_back({static_cast<uint32_t>(attachDescs.size()-1),
                              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
      }
    };

    add(*colors);

    if (resolves) {
      if (colors->size() != resolves->size())
        throw invalid_argument("Pass color/resolve attachment mismatch");

      add(*resolves);
    }
  }

  if (depthStencil) {
    auto aspect = aspectOfVK(depthStencil->format);
    if (!(aspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)))
      throw invalid_argument("Invalid format for depth/stencil attachment");

    attachDescs.push_back({});

    auto& desc = attachDescs.back();
    desc.flags = 0;
    desc.format = toFormatVK(depthStencil->format);
    desc.samples = toSampleCountVK(depthStencil->samples);
    desc.loadOp = toLoadOpVK(depthStencil->depLoadOp);
    desc.storeOp = toStoreOpVK(depthStencil->depStoreOp);
    desc.stencilLoadOp = toLoadOpVK(depthStencil->stenLoadOp);
    desc.stencilStoreOp = toStoreOpVK(depthStencil->stenStoreOp);
    // XXX: won't work if an image view is used in more than one pass
    desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    desc.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    attachRefs.push_back({static_cast<uint32_t>(attachDescs.size()-1),
                          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL});
  }

  // Define subpass
  VkSubpassDescription subDesc;
  subDesc.flags = 0;
  subDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subDesc.inputAttachmentCount = 0;
  subDesc.pInputAttachments = nullptr;
  subDesc.preserveAttachmentCount = 0;
  subDesc.pPreserveAttachments = nullptr;

  if (colors) {
    subDesc.colorAttachmentCount = colors->size();
    subDesc.pColorAttachments = attachRefs.data();
    if (resolves)
      subDesc.pResolveAttachments = attachRefs.data()+colors->size();
    else
      subDesc.pResolveAttachments = nullptr;
  } else {
    subDesc.colorAttachmentCount = 0;
    subDesc.pColorAttachments = nullptr;
    subDesc.pResolveAttachments = nullptr;
  }

  if (depthStencil)
    subDesc.pDepthStencilAttachment = attachRefs.data()+attachRefs.size()-1;
  else
    subDesc.pDepthStencilAttachment = nullptr;

  // Create render pass
  VkRenderPassCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.attachmentCount = attachDescs.size();
  info.pAttachments = attachDescs.data();
  info.subpassCount = 1;
  info.pSubpasses = &subDesc;
  info.dependencyCount = 0;
  info.pDependencies = nullptr;

  auto dev = DeviceVK::get().device();
  auto res = vkCreateRenderPass(dev, &info, nullptr, &renderPass_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create render pass");
}

PassVK::~PassVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyRenderPass(dev, renderPass_, nullptr);
}

Target::Ptr PassVK::target(Size2 size, uint32_t layers,
                           const vector<AttachImg>* colors,
                           const vector<AttachImg>* resolves,
                           const AttachImg* depthStencil) {

  return make_unique<TargetVK>(*this, size, layers, colors, resolves,
                               depthStencil);
}

VkRenderPass PassVK::renderPass() const {
  return renderPass_;
}

// ------------------------------------------------------------------------
// TargetVK

TargetVK::TargetVK(PassVK& pass, Size2 size, uint32_t layers,
                   const vector<AttachImg>* colors,
                   const vector<AttachImg>* resolves,
                   const AttachImg* depthStencil)
  : Target(size, layers, colors, resolves, depthStencil), pass_(pass) {

  if (size == 0 || layers == 0)
    throw invalid_argument("TargetVK requires size > 0 and layers > 0");

  // TODO: consider relaxing compatibility requirements

  // Define attachments
  if (colors) {
    auto add = [&](const vector<AttachImg>& attachs) {
      for (const auto& a : attachs) {
        auto img = static_cast<ImageVK*>(a.image);
        views_.push_back(img->getView(a.layer, layers, a.level, 1));
      }
    };

    if (!pass_.colors_ || pass_.colors_->size() != colors->size())
      throw invalid_argument("Target not compatible with pass");

    add(*colors);

    if (resolves) {
      if (!pass_.resolves_ || pass_.resolves_->size() != resolves->size())
        throw invalid_argument("Target not compatible with pass");

      add(*resolves);
    }

  } else if (pass_.colors_) {
    throw invalid_argument("Target not compatible with pass");
  }

  if (depthStencil_) {
    if (!pass_.depthStencil_)
      throw invalid_argument("Target not compatible with pass");

    auto img = static_cast<ImageVK*>(depthStencil->image);
    views_.push_back(img->getView(depthStencil->layer, layers,
                                  depthStencil->level, 1));

  } else if (pass_.depthStencil_) {
    throw invalid_argument("Target not compatible with pass");
  }

  vector<VkImageView> attachs;
  for (const auto& v : views_)
    attachs.push_back(v->handle());

  // Create framebuffer
  VkFramebufferCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.renderPass = pass_.renderPass();
  info.attachmentCount = attachs.size();
  info.pAttachments = attachs.data();
  info.width = size.width;
  info.height = size.height;
  info.layers = layers;

  auto dev = DeviceVK::get().device();
  auto res = vkCreateFramebuffer(dev, &info, nullptr, &framebuffer_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create framebuffer");
}

TargetVK::~TargetVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyFramebuffer(dev, framebuffer_, nullptr);
}

Pass& TargetVK::pass() const {
  return pass_;
}

VkFramebuffer TargetVK::framebuffer() const {
  return framebuffer_;
}
