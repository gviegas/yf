//
// CG
// DcTableVK.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_DCTABLEVK_H
#define YF_CG_DCTABLEVK_H

#include <vector>
#include <unordered_map>

#include "DcTable.h"
#include "VK.h"
#include "ImageVK.h"

CG_NS_BEGIN

class DcTableVK final : public DcTable {
 public:
  explicit DcTableVK(const std::vector<DcEntry>& entries);
  ~DcTableVK();
  void allocate(uint32_t n);
  uint32_t allocations() const;

  void write(uint32_t allocation, DcId id, uint32_t element,
             Buffer& buffer, uint64_t offset, uint64_t size);

  void write(uint32_t allocation, DcId id, uint32_t element,
             Image& image, uint32_t layer, uint32_t level);

  void write(uint32_t allocation, DcId id, uint32_t element,
             Image& image, uint32_t layer, uint32_t level,
             Sampler& sampler);

  /// Getters.
  ///
  VkDescriptorSetLayout dsLayout();
  VkDescriptorSet ds(uint32_t index);

 private:
  VkDescriptorSetLayout dsLayout_ = VK_NULL_HANDLE;
  VkDescriptorPool pool_ = VK_NULL_HANDLE;
  std::vector<VkDescriptorPoolSize> poolSizes_{};
  std::vector<VkDescriptorSet> sets_{};

  void write(uint32_t allocation, DcId id, uint32_t element,
             Image& image, uint32_t layer, uint32_t level,
             Sampler* sampler);

  /// Image views and sampler objects used in `write()`s are managed by
  /// the table. Every image/sampler descriptor has a list of `elements`
  /// size holding resources used in the most recent update. A list of
  /// `allocations` size holds descriptor-to-resource-list mappings.
  ///
  struct ImgRef {
    ImageVK::View::Ptr view;
    SamplerVK::Ptr sampler;
  };

  using ImgRefs = std::unordered_map<DcId, std::vector<ImgRef>>;
  std::vector<ImgRefs> imgRefs_{};

  void resetImgRefs();
};

CG_NS_END

#endif // YF_CG_DCTABLEVK_H
