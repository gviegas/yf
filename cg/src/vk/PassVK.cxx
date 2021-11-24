//
// CG
// PassVK.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cassert>

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

  try {
    if (colors) {
      for (const auto& color : *colors) {
        if (aspectOfVK(color.format) != VK_IMAGE_ASPECT_COLOR_BIT)
          throw invalid_argument("Invalid format for color attachment");
      }
      colors_ = new auto(*colors);

      if (resolves) {
        if (colors->size() != resolves->size())
          throw invalid_argument("Pass color/resolve attachment mismatch");

        for (size_t i = 0; i < colors->size(); i++) {
          if ((*colors)[i].format != (*resolves)[i].format)
            throw invalid_argument("Pass color/resolve attachment mismatch");
        }
        resolves_ = new auto(*resolves);
      }
    } else if (resolves) {
      throw invalid_argument("Pass color/resolve attachment mismatch");
    }

    if (depthStencil) {
      auto aspect = aspectOfVK(depthStencil->format);
      if (!(aspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)))
        throw invalid_argument("Invalid format for depth/stencil attachment");

      depthStencil_ = new auto(*depthStencil);
    }
  } catch (const bad_alloc&) {
    delete colors_;
    delete resolves_;
    delete depthStencil_;
    throw;
  }
}

PassVK::~PassVK() {
  delete colors_;
  delete resolves_;
  delete depthStencil_;

  // TODO: Notify
  auto dev = deviceVK().device();
  for (auto& rp : renderPasses_)
    vkDestroyRenderPass(dev, rp.renderPass, nullptr);
}

void PassVK::setColors(vector<VkAttachmentDescription>& descs,
                       vector<VkAttachmentReference>& refs,
                       const vector<LoadStoreOp>& ops) {
  auto op = ops.begin();

  for (const auto& color : *colors_) {
    descs.push_back({});
    auto& desc = descs.back();
    desc.flags = 0;
    desc.format = toFormatVK(color.format);
    desc.samples = toSampleCountVK(color.samples);
    desc.loadOp = op->first;
    desc.storeOp = op->second;
    desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    if (op->first != VK_ATTACHMENT_LOAD_OP_LOAD)
      desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    else
      desc.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
    desc.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    refs.push_back({
      static_cast<uint32_t>(descs.size() - 1),
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    });

    op++;
  }
}

void PassVK::setDepthStencil(vector<VkAttachmentDescription>& descs,
                             vector<VkAttachmentReference>& refs,
                             LoadStoreOp depthOp, LoadStoreOp stencilOp) {
  descs.push_back({});
  auto& desc = descs.back();
  desc.flags = 0;
  desc.format = toFormatVK(depthStencil_->format);
  desc.samples = toSampleCountVK(depthStencil_->samples);
  desc.loadOp = depthOp.first;
  desc.storeOp = depthOp.second;
  desc.stencilLoadOp = stencilOp.first;
  desc.stencilStoreOp = stencilOp.second;
  if (depthOp.first != VK_ATTACHMENT_LOAD_OP_LOAD &&
      stencilOp.first != VK_ATTACHMENT_LOAD_OP_LOAD)
    desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  else
    desc.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
  desc.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

  refs.push_back({
    static_cast<uint32_t>(descs.size() - 1),
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
  });
}

void PassVK::setSubpass(VkSubpassDescription& subpass,
                        const vector<VkAttachmentReference>& refs) {
  subpass.flags = 0;
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.inputAttachmentCount = 0;
  subpass.pInputAttachments = nullptr;
  if (colors_) {
    subpass.colorAttachmentCount = colors_->size();
    subpass.pColorAttachments = refs.data();
  } else {
    subpass.colorAttachmentCount = 0;
    subpass.pColorAttachments = nullptr;
  }
  subpass.pResolveAttachments = nullptr;
  if (depthStencil_)
    subpass.pDepthStencilAttachment = &refs.data()[refs.size() - 1];
  else
    subpass.pDepthStencilAttachment = nullptr;
  subpass.preserveAttachmentCount = 0;
  subpass.pPreserveAttachments = nullptr;
}

VkRenderPass PassVK::createRenderPass(
  const vector<VkAttachmentDescription>& descs,
  const VkSubpassDescription& subpass) {

  VkRenderPassCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.attachmentCount = descs.size();
  info.pAttachments = descs.data();
  info.subpassCount = 1;
  info.pSubpasses = &subpass;
  info.dependencyCount = 0;
  info.pDependencies = nullptr;

  VkRenderPass renderPass;
  VkResult res = vkCreateRenderPass(deviceVK().device(), &info, nullptr,
                                    &renderPass);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create render pass");
  return renderPass;
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
  const LoadStoreOp op{
    VK_ATTACHMENT_LOAD_OP_LOAD,//CLEAR,
    VK_ATTACHMENT_STORE_OP_STORE
  };
  if (colors_)
    return renderPass(vector<LoadStoreOp>(colors_->size(), op), op, op);
  return renderPass({}, op, op);
}

VkRenderPass PassVK::renderPass(const vector<LoadStoreOp>& colors,
                                LoadStoreOp depth, LoadStoreOp stencil) {
  RenderPass* renderPass = &renderPasses_[0];
  for (auto& rp : renderPasses_) {
    if (rp.renderPass == VK_NULL_HANDLE) {
      renderPass = &rp;
      break;
    }
    if (rp.equalOp(colors, depth, stencil))
      return rp.renderPass;
  }

  // Need a new render pass for these operations
  vector<VkAttachmentDescription> descs;
  vector<VkAttachmentReference> refs;
  VkSubpassDescription subpass;
  if (colors_) {
    assert(colors.size() == colors_->size());
    setColors(descs, refs, colors);
  }
  if (depthStencil_)
    setDepthStencil(descs, refs, depth, stencil);
  setSubpass(subpass, refs);

  vkDestroyRenderPass(deviceVK().device(), renderPass->renderPass, nullptr);
  renderPass->renderPass = VK_NULL_HANDLE;
  renderPass->renderPass = createRenderPass(descs, subpass);
  renderPass->colors = colors;
  renderPass->depth = depth;
  renderPass->stencil = stencil;
  return renderPass->renderPass;
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
