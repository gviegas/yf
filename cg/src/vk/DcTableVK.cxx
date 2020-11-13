//
// CG
// DcTableVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "DcTableVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

DcTableVK::DcTableVK(const DcEntries& entries) : DcTable(entries) {
  if (entries.empty())
    throw invalid_argument("DcTableVK requires entries to be non-empty");

  vector<VkDescriptorSetLayoutBinding> binds;
  VkDescriptorType type;
  uint32_t unifN  = 0;
  uint32_t storN  = 0;
  uint32_t imgN   = 0;
  uint32_t ismplN = 0;

  for (const auto& e : entries) {
    if (e.second.elements == 0)
      throw invalid_argument("DcEntry requires elements > 0");

    switch (e.second.type) {
    case DcTypeUniform:
      type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      unifN += e.second.elements;
      break;
    case DcTypeStorage:
      type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      storN += e.second.elements;
      break;
    case DcTypeImage:
      type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      imgN += e.second.elements;
      break;
    case DcTypeImgSampler:
      type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      ismplN += e.second.elements;
      break;
    }

    binds.push_back({e.first, type, e.second.elements,
                     VK_SHADER_STAGE_ALL, nullptr});
  }

  VkDescriptorSetLayoutCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.bindingCount = binds.size();
  info.pBindings = binds.data();

  auto dev = DeviceVK::get().device();
  auto res = vkCreateDescriptorSetLayout(dev, &info, nullptr, &dsLayout_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create descriptor set layout");

  if (unifN > 0)
    poolSizes_.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, unifN});
  if (storN > 0)
    poolSizes_.push_back({VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, storN});
  if (imgN > 0)
    poolSizes_.push_back({VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, imgN});
  if (ismplN > 0)
    poolSizes_.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ismplN});
}

DcTableVK::~DcTableVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyDescriptorPool(dev, pool_, nullptr);
  vkDestroyDescriptorSetLayout(dev, dsLayout_, nullptr);
}

void DcTableVK::allocate(uint32_t n) {
  // TODO
  throw runtime_error("Unimplemented");
}

uint32_t DcTableVK::allocations() const {
  return sets_.size();
}

void DcTableVK::write(uint32_t allocation,
                      DcId id,
                      uint32_t element,
                      Buffer& buffer,
                      uint64_t offset,
                      uint64_t size) {

  // TODO
  throw runtime_error("Unimplemented");
}

void DcTableVK::write(uint32_t allocation,
                      DcId id,
                      uint32_t element,
                      Image& image,
                      uint32_t layer) {

  // TODO
  throw runtime_error("Unimplemented");
}
