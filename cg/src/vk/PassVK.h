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
};

CG_NS_END

#endif // YF_CG_PASSVK_H
