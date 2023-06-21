//
// CG
// PassVK.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#ifndef YF_CG_PASSVK_H
#define YF_CG_PASSVK_H

#include <algorithm>
#include <utility>
#include <array>
#include <stdexcept>

#include "Defs.h"
#include "Pass.h"
#include "Encoder.h"
#include "VK.h"
#include "ImageVK.h"

CG_NS_BEGIN

class TargetVK;

class PassVK final : public Pass {
 public:
  PassVK(const std::vector<AttachDesc>* colors,
         const std::vector<AttachDesc>* resolves,
         const AttachDesc* depthStencil);

  ~PassVK();

  Target::Ptr target(Size2 size, uint32_t layers,
                     const std::vector<AttachImg>* colors,
                     const std::vector<AttachImg>* resolves,
                     const AttachImg* depthStencil);

  const std::vector<AttachDesc>* colors() const;
  const std::vector<AttachDesc>* resolves() const;
  const AttachDesc* depthStencil() const;

  /// Getters.
  ///
  VkRenderPass renderPass();
  VkRenderPass renderPass(const std::vector<LoadStoreOp>& colors,
                          LoadStoreOp depth, LoadStoreOp stencil);

 private:
  /// Render pass handle created with given attachment operations.
  ///
  struct RenderPass {
    VkRenderPass renderPass = VK_NULL_HANDLE;
    std::vector<LoadStoreOp> colors{};
    LoadStoreOp depth{};
    LoadStoreOp stencil{};

    bool equalOp(const std::vector<LoadStoreOp>& otherColors,
                 LoadStoreOp otherDepth, LoadStoreOp otherStencil) {
      return depth == otherDepth && stencil == otherStencil &&
             std::equal(colors.begin(), colors.end(), otherColors.begin());
    }

    bool operator==(const RenderPass& other) {
      return renderPass != VK_NULL_HANDLE &&
             equalOp(other.colors, other.depth, other.stencil);
    }
  };

  std::vector<AttachDesc>* colors_{};
  std::vector<AttachDesc>* resolves_{};
  AttachDesc* depthStencil_{};
  std::array<RenderPass, 4> renderPasses_{};

  void setColors(std::vector<VkAttachmentDescription>&,
                 std::vector<VkAttachmentReference>&,
                 const std::vector<LoadStoreOp>&);

  void setDepthStencil(std::vector<VkAttachmentDescription>&,
                       std::vector<VkAttachmentReference>&,
                       LoadStoreOp, LoadStoreOp);

  void setSubpass(VkSubpassDescription&,
                  const std::vector<VkAttachmentReference>&);

  VkRenderPass createRenderPass(const std::vector<VkAttachmentDescription>&,
                                const VkSubpassDescription&);
};

class TargetVK final : public Target {
 public:
  TargetVK(PassVK& pass, Size2 size, uint32_t layers,
           const std::vector<AttachImg>* colors,
           const std::vector<AttachImg>* resolves,
           const AttachImg* depthStencil);

  ~TargetVK();

  Pass& pass();
  Size2 size() const;
  uint32_t layers() const;
  const std::vector<AttachImg>* colors() const;
  const std::vector<AttachImg>* resolves() const;
  const AttachImg* depthStencil() const;

  /// Getter.
  ///
  VkFramebuffer framebuffer();

  /// Sets a `VkRenderPassBeginInfo` for a given `TargetOp'.
  ///
  void setBeginInfo(VkRenderPassBeginInfo& info,
                    std::vector<VkClearValue>& clearValues,
                    const TargetOp& targetOp);

 private:
  PassVK& pass_;
  const Size2 size_{0, 0};
  const uint32_t layers_{};
  std::vector<AttachImg>* colors_{};
  std::vector<AttachImg>* resolves_{};
  AttachImg* depthStencil_{};
  VkFramebuffer framebuffer_ = VK_NULL_HANDLE;
  std::vector<ImageVK::View::Ptr> views_{};

  void createColorViews(std::vector<VkImageView>&);
  void createDepthStencilView(std::vector<VkImageView>&);
  void createFramebuffer(const std::vector<VkImageView>&);
};

/// Converts from a LoadOp value.
///
inline VkAttachmentLoadOp toLoadOpVK(LoadOp op) {
  switch (op) {
  case LoadOpClear:    return VK_ATTACHMENT_LOAD_OP_CLEAR;
  case LoadOpLoad:     return VK_ATTACHMENT_LOAD_OP_LOAD;
  case LoadOpDontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  default:             throw std::invalid_argument("Invalid LoadOp value");
  }
}

/// Converts from a StoreOp value.
///
inline VkAttachmentStoreOp toStoreOpVK(StoreOp op) {
  switch (op) {
  case StoreOpStore:    return VK_ATTACHMENT_STORE_OP_STORE;
  case StoreOpDontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
  default:              throw std::invalid_argument("Invalid StoreOp value");
  }
}

CG_NS_END

#endif // YF_CG_PASSVK_H
