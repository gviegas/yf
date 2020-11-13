//
// CG
// DcTableVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_DCTABLEVK_H
#define YF_CG_DCTABLEVK_H

#include <vector>

#include "DcTable.h"
#include "VK.h"

CG_NS_BEGIN

class DcTableVK final : public DcTable {
 public:
  explicit DcTableVK(const DcEntries& entries);
  ~DcTableVK();
  void allocate(uint32_t n);
  uint32_t allocations() const;

  void write(uint32_t allocation,
             DcId id,
             uint32_t element,
             Buffer& buffer,
             uint64_t offset,
             uint64_t size);

  void write(uint32_t allocation,
             DcId id,
             uint32_t element,
             Image& image,
             uint32_t layer);

 private:
  VkDescriptorSetLayout dsLayout_ = VK_NULL_HANDLE;
  VkDescriptorPool pool_ = VK_NULL_HANDLE;
  std::vector<VkDescriptorPoolSize> poolSizes_{};
  std::vector<VkDescriptorSet> sets_{};
};

CG_NS_END

#endif // YF_CG_DCTABLEVK_H
