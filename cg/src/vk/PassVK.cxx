//
// CG
// PassVK.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "PassVK.h"
#include "ImageVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

//
// PassVK
//

PassVK::PassVK(const vector<AttachDesc>* colors,
               const vector<AttachDesc>* resolves,
               const AttachDesc* depthStencil) {

  const auto& lim = deviceVK().physLimits();
  if (colors && colors->size() > lim.maxColorAttachments)
    throw invalid_argument("Too many color attachments");

  vector<VkAttachmentDescription> attachDescs;
  vector<VkAttachmentReference> attachRefs;

  // Define attachments
  if (colors) {
    auto add = [&](const vector<AttachDesc>& attachs) {
      for (const auto& attach : attachs) {
        if (aspectOfVK(attach.format) != VK_IMAGE_ASPECT_COLOR_BIT)
          throw invalid_argument("Invalid format for color attachment");

        attachDescs.push_back({});

        auto& desc = attachDescs.back();
        desc.flags = 0;
        desc.format = toFormatVK(attach.format);
        desc.samples = toSampleCountVK(attach.samples);
        desc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        // FIXME: Won't work if an image view is used in more than one pass
        desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        desc.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

        attachRefs.push_back({static_cast<uint32_t>(attachDescs.size()-1),
                              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
      }
    };

    colors_ = new auto(*colors);
    add(*colors);

    if (resolves) {
      if (colors->size() != resolves->size())
        throw invalid_argument("Pass color/resolve attachment mismatch");

      resolves_ = new auto(*resolves);
      add(*resolves);
    }
  }

  if (depthStencil) {
    auto aspect = aspectOfVK(depthStencil->format);
    if (!(aspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)))
      throw invalid_argument("Invalid format for depth/stencil attachment");

    depthStencil_ = new auto(*depthStencil);

    attachDescs.push_back({});

    auto& desc = attachDescs.back();
    desc.flags = 0;
    desc.format = toFormatVK(depthStencil->format);
    desc.samples = toSampleCountVK(depthStencil->samples);
    desc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    // FIXME: Won't work if an image view is used in more than one pass
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

  auto dev = deviceVK().device();
  auto res = vkCreateRenderPass(dev, &info, nullptr, &renderPass_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create render pass");
}

PassVK::~PassVK() {
  delete colors_;
  delete resolves_;
  delete depthStencil_;

  // TODO: Notify
  auto dev = deviceVK().device();
  vkDestroyRenderPass(dev, renderPass_, nullptr);
}

Target::Ptr PassVK::target(Size2 size, uint32_t layers,
                           const vector<AttachImg>* colors,
                           const vector<AttachImg>* resolves,
                           const AttachImg* depthStencil) {

  return make_unique<TargetVK>(*this, size, layers, colors, resolves,
                               depthStencil);
}

const vector<AttachDesc>* PassVK::colors() const {
  return colors_;
}

const vector<AttachDesc>* PassVK::resolves() const {
  return resolves_;
}

const AttachDesc* PassVK::depthStencil() const {
  return depthStencil_;
}

VkRenderPass PassVK::renderPass() {
  return renderPass_;
}

//
// TargetVK
//

TargetVK::TargetVK(PassVK& pass, Size2 size, uint32_t layers,
                   const vector<AttachImg>* colors,
                   const vector<AttachImg>* resolves,
                   const AttachImg* depthStencil)
  : pass_(pass), size_(size), layers_(layers) {

  if (size == 0 || layers == 0)
    throw invalid_argument("TargetVK requires size > 0 and layers > 0");

  const auto& lim = deviceVK().physLimits();
  if (size.width > lim.maxFramebufferWidth ||
      size.height > lim.maxFramebufferHeight ||
      layers > lim.maxFramebufferLayers)
    throw invalid_argument("TargetVK limit");

  // TODO: Consider relaxing compatibility requirements

  // Define attachments
  if (colors) {
    auto add = [&](const vector<AttachImg>& attachs) {
      for (const auto& a : attachs) {
        auto img = static_cast<ImageVK*>(a.image);
        views_.push_back(img->getView(a.layer, layers, a.level, 1));
      }
    };

    if (!pass.colors() || pass.colors()->size() != colors->size())
      throw invalid_argument("Target not compatible with pass");

    colors_ = new auto(*colors);
    add(*colors);

    if (resolves) {
      if (!pass.resolves() || pass.resolves()->size() != resolves->size())
        throw invalid_argument("Target not compatible with pass");

      resolves_ = new auto(*resolves);
      add(*resolves);
    }

  } else if (pass.colors()) {
    throw invalid_argument("Target not compatible with pass");
  }

  if (depthStencil) {
    if (!pass.depthStencil())
      throw invalid_argument("Target not compatible with pass");

    depthStencil_ = new auto(*depthStencil);

    auto img = static_cast<ImageVK*>(depthStencil->image);
    views_.push_back(img->getView(depthStencil->layer, layers,
                                  depthStencil->level, 1));

  } else if (pass.depthStencil()) {
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
  info.renderPass = pass.renderPass();
  info.attachmentCount = attachs.size();
  info.pAttachments = attachs.data();
  info.width = size.width;
  info.height = size.height;
  info.layers = layers;

  auto dev = deviceVK().device();
  auto res = vkCreateFramebuffer(dev, &info, nullptr, &framebuffer_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create framebuffer");
}

TargetVK::~TargetVK() {
  delete colors_;
  delete resolves_;
  delete depthStencil_;

  // TODO: Notify
  auto dev = deviceVK().device();
  vkDestroyFramebuffer(dev, framebuffer_, nullptr);
}

Pass& TargetVK::pass() {
  return pass_;
}

Size2 TargetVK::size() const {
  return size_;
}

uint32_t TargetVK::layers() const {
  return layers_;
}

const vector<AttachImg>* TargetVK::colors() const {
  return colors_;
}

const vector<AttachImg>* TargetVK::resolves() const {
  return resolves_;
}

const AttachImg* TargetVK::depthStencil() const {
  return depthStencil_;
}

VkFramebuffer TargetVK::framebuffer() {
  return framebuffer_;
}
