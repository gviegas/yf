//
// CG
// PassVK.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_PASSVK_H
#define YF_CG_PASSVK_H

#include <stdexcept>

#include "Defs.h"
#include "Pass.h"
#include "VK.h"
#include "ImageVK.h"

CG_NS_BEGIN

class TargetVK;

class PassVK final : public Pass {
 public:
  PassVK(const std::vector<ColorAttach>* colors,
         const std::vector<ColorAttach>* resolves,
         const DepStenAttach* depthStencil);

  ~PassVK();

  Target::Ptr target(Size2 size, uint32_t layers,
                     const std::vector<AttachImg>* colors,
                     const std::vector<AttachImg>* resolves,
                     const AttachImg* depthStencil);

  /// Getter.
  ///
  VkRenderPass renderPass();

 private:
  VkRenderPass renderPass_ = VK_NULL_HANDLE;
};

class TargetVK final : public Target {
 public:
  TargetVK(PassVK& pass, Size2 size, uint32_t layers,
           const std::vector<AttachImg>* colors,
           const std::vector<AttachImg>* resolves,
           const AttachImg* depthStencil);

  ~TargetVK();

  Pass& pass();

  /// Getter.
  ///
  VkFramebuffer framebuffer();

 private:
  PassVK& pass_;
  VkFramebuffer framebuffer_ = VK_NULL_HANDLE;
  std::vector<ImageVK::View::Ptr> views_{};
};

/// Converts from a `LoadOP` value.
///
inline VkAttachmentLoadOp toLoadOpVK(LoadOp op) {
  switch (op) {
  case LoadOpLoad:     return VK_ATTACHMENT_LOAD_OP_LOAD;
  //case LoadOpClear:    return VK_ATTACHMENT_LOAD_OP_CLEAR;
  case LoadOpDontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `StoreOp` value.
///
inline VkAttachmentStoreOp toStoreOpVK(StoreOp op) {
  switch (op) {
  case StoreOpStore:    return VK_ATTACHMENT_STORE_OP_STORE;
  case StoreOpDontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
  default:
    throw std::invalid_argument(__func__);
  }
}

CG_NS_END

#endif // YF_CG_PASSVK_H
