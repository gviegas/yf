//
// CG
// DcTableVK.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "DcTableVK.h"
#include "BufferVK.h"
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

  auto dev = deviceVK().device();
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
  auto dev = deviceVK().device();
  vkDestroyDescriptorPool(dev, pool_, nullptr);
  vkDestroyDescriptorSetLayout(dev, dsLayout_, nullptr);
}

void DcTableVK::allocate(uint32_t n) {
  // XXX: since currently held resources will only be freed after (and if)
  // the new allocation succeeds, one may need to call `allocate(0)` first

  if (n == sets_.size())
    return;

  auto dev = deviceVK().device();

  if (n == 0) {
    vkDestroyDescriptorPool(dev, pool_, nullptr);
    pool_ = VK_NULL_HANDLE;
    sets_.clear();
    imgRefs_.clear();
    return;
  }

  // Create new pool
  auto sizes = poolSizes_;
  if (n > 1) {
    for (auto& s : sizes)
      s.descriptorCount *= n;
  }
  VkDescriptorPool pool;
  VkResult res;

  VkDescriptorPoolCreateInfo poolInfo;
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.pNext = nullptr;
  poolInfo.flags = 0;
  poolInfo.maxSets = n;
  poolInfo.poolSizeCount = sizes.size();
  poolInfo.pPoolSizes = sizes.data();

  res = vkCreateDescriptorPool(dev, &poolInfo, nullptr, &pool);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create descriptor pool");

  // Allocate new descriptor sets
  vector<VkDescriptorSetLayout> layouts(n, dsLayout_);
  vector<VkDescriptorSet> sets;
  sets.resize(n);

  VkDescriptorSetAllocateInfo allocInfo;
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.pNext = nullptr;
  allocInfo.descriptorPool = pool;
  allocInfo.descriptorSetCount = n;
  allocInfo.pSetLayouts = layouts.data();

  res = vkAllocateDescriptorSets(dev, &allocInfo, sets.data());
  if (res != VK_SUCCESS) {
    vkDestroyDescriptorPool(dev, pool, nullptr);
    throw DeviceExcept("Could not allocate descriptor set(s)");
  }

  vkDestroyDescriptorPool(dev, pool_, nullptr);
  pool_ = pool;
  sets_ = sets;
  resetImgRefs();
}

uint32_t DcTableVK::allocations() const {
  return sets_.size();
}

void DcTableVK::write(uint32_t allocation, DcId id, uint32_t element,
                      Buffer& buffer, uint64_t offset, uint64_t size) {

  auto ent = entries_.find(id);

  if (allocation >= sets_.size() ||
      ent == entries_.end() ||
      (ent->second.type != DcTypeUniform &&
        ent->second.type != DcTypeStorage) ||
      element >= ent->second.elements ||
      offset + size > buffer.size_)
    throw invalid_argument("DcTable write() [Buffer]");

  VkDescriptorBufferInfo info;
  info.buffer = static_cast<BufferVK&>(buffer).handle();
  info.offset = offset;
  info.range = size;

  VkWriteDescriptorSet wr;
  wr.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  wr.pNext = nullptr;
  wr.dstSet = sets_[allocation];
  wr.dstBinding = id;
  wr.dstArrayElement = element;
  wr.descriptorCount = 1;
  if (ent->second.type == DcTypeUniform)
    wr.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  else
    wr.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  wr.pImageInfo = nullptr;
  wr.pBufferInfo = &info;
  wr.pTexelBufferView = nullptr;

  vkUpdateDescriptorSets(deviceVK().device(), 1, &wr, 0, nullptr);
}

void DcTableVK::write(uint32_t allocation, DcId id, uint32_t element,
                      Image& image, uint32_t layer, uint32_t level,
                      ImgSampler sampler) {

  auto ent = entries_.find(id);

  if (allocation >= sets_.size() ||
      ent == entries_.end() ||
      (ent->second.type != DcTypeImage &&
        ent->second.type != DcTypeImgSampler) ||
      element >= ent->second.elements ||
      layer >= image.layers_)
    throw invalid_argument("DcTableVK write() [Image]");

  ImgRef& ref = imgRefs_[allocation].find(id)->second[element];

  // Check if view object can be reused
  if (!ref.view || &ref.view->image() != &image ||
      ref.view->firstLayer() != layer || ref.view->firstLevel() != level)
    ref.view = static_cast<ImageVK&>(image).getView(layer, 1, level, 1);

  VkDescriptorImageInfo info;
  info.imageView = ref.view->handle();
  info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

  VkWriteDescriptorSet wr;
  wr.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  wr.pNext = nullptr;
  wr.dstSet = sets_[allocation];
  wr.dstBinding = id;
  wr.dstArrayElement = element;
  wr.descriptorCount = 1;
  wr.pImageInfo = &info;
  wr.pBufferInfo = nullptr;
  wr.pTexelBufferView = nullptr;

  // Check if sampler is needed
  switch (ent->second.type) {
  case DcTypeImage:
    ref.sampler = nullptr;
    info.sampler = VK_NULL_HANDLE;
    wr.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    break;
  case DcTypeImgSampler:
    if (!ref.sampler || ref.sampler->type() != sampler)
      ref.sampler = SamplerVK::make(sampler);
    info.sampler = ref.sampler->handle();
    wr.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    break;
  default:
    break;
  }

  vkUpdateDescriptorSets(deviceVK().device(), 1, &wr, 0, nullptr);
}

void DcTableVK::resetImgRefs() {
  imgRefs_.clear();

  // Cannot copy unique pointers
  for (auto i = sets_.size(); i > 0; --i) {
    imgRefs_.push_back({});

    for (const auto& e : entries_) {
      if (e.second.type != DcTypeImgSampler && e.second.type != DcTypeImage)
        continue;

      auto it = imgRefs_.back().emplace(e.first, vector<ImgRef>()).first;

      for (auto j = e.second.elements; j > 0; --j)
        it->second.push_back({nullptr, nullptr});
    }

    // TODO: do this once
    if (imgRefs_[0].empty()) {
      imgRefs_.clear();
      return;
    }
  }
}

VkDescriptorSetLayout DcTableVK::dsLayout() const {
  return dsLayout_;
}

VkDescriptorSet DcTableVK::ds(uint32_t index) const {
  if (index >= sets_.size())
    throw invalid_argument("Descriptor set index out of range");

  return sets_[index];
}
