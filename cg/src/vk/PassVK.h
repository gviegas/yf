//
// CG
// PassVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_PASSVK_H
#define YF_CG_PASSVK_H

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

  Target::Ptr makeTarget(Size2 size,
                         uint32_t layers,
                         const std::vector<AttachImg>* colors,
                         const std::vector<AttachImg>* resolves,
                         const AttachImg* depthStencil);

  /// Getter.
  ///
  VkRenderPass renderPass() const;

 private:
  VkRenderPass renderPass_ = VK_NULL_HANDLE;
};

class TargetVK final : public Target {
 public:
  TargetVK(PassVK& pass,
           Size2 size,
           uint32_t layers,
           const std::vector<AttachImg>* colors,
           const std::vector<AttachImg>* resolves,
           const AttachImg* depthStencil);

  ~TargetVK();

  Pass& pass() const;

  /// Getter.
  ///
  VkFramebuffer framebuffer() const;

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
  case LoadOpClear:    return VK_ATTACHMENT_LOAD_OP_CLEAR;
  case LoadOpDontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  }
}

/// Converts from a `StoreOp` value.
///
inline VkAttachmentStoreOp toStoreOpVK(StoreOp op) {
  switch (op) {
  case StoreOpStore:    return VK_ATTACHMENT_STORE_OP_STORE;
  case StoreOpDontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
  }
}

CG_NS_END

#endif // YF_CG_PASSVK_H
