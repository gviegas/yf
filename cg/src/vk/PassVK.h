//
// cg
// PassVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_PASSVK_H
#define YF_CG_PASSVK_H

#include "yf/cg/Defs.h"
#include "yf/cg/Pass.h"
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
  VkRenderPass _renderPass = VK_NULL_HANDLE;
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
  const Pass& pass() const;

  VkFramebuffer framebuffer() const;

 private:
  PassVK& _pass;
  VkFramebuffer _framebuffer = VK_NULL_HANDLE;
};

CG_NS_END

#endif // YF_CG_PASSVK_H
