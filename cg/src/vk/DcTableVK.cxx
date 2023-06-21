//
// CG
// DcTableVK.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#include <algorithm>
#include <stdexcept>

#include "DcTableVK.h"
#include "BufferVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

DcTableVK::DcTableVK(const vector<DcEntry>& entries) : entries_(entries) {
  if (entries.empty())
    throw invalid_argument("DcTableVK requires entries to be non-empty");

  sort(entries_.begin(), entries_.end(),
       [](auto& a, auto& b) { return a.id < b.id; });

  vector<VkDescriptorSetLayoutBinding> binds;
  VkDescriptorType type;
  uint32_t unifN = 0;
  uint32_t storN = 0;
  uint32_t imgN = 0;
  uint32_t isplrN = 0;

  for (const auto& e : entries_) {
    if (e.elements == 0)
      throw invalid_argument("DcEntry requires elements > 0");

    switch (e.type) {
    case DcTypeUniform:
      type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      unifN += e.elements;
      break;
    case DcTypeStorage:
      type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      storN += e.elements;
      break;
    case DcTypeImage:
      type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      imgN += e.elements;
      break;
    case DcTypeImgSampler:
      type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      isplrN += e.elements;
      break;
    default:
      throw invalid_argument("Invalid DcType value");
    }

    binds.push_back({e.id, type, e.elements, VK_SHADER_STAGE_ALL, nullptr});
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
  if (isplrN > 0)
    poolSizes_.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, isplrN});
}

DcTableVK::~DcTableVK() {
  // TODO: Notify
  auto dev = deviceVK().device();
  vkDestroyDescriptorPool(dev, pool_, nullptr);
  vkDestroyDescriptorSetLayout(dev, dsLayout_, nullptr);
}

void DcTableVK::allocate(uint32_t n) {
  // XXX: Since currently held resources will only be freed after (and if)
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

  const auto ent = lower_bound(entries_.begin(), entries_.end(),
                               DcEntry{id, DcTypeUniform, 0},
                               [](auto& a, auto& b) { return a.id < b.id; });

  if (allocation >= sets_.size() || ent == entries_.end() || ent->id != id ||
      (ent->type != DcTypeUniform && ent->type != DcTypeStorage) ||
      element >= ent->elements || offset + size > buffer.size())
    throw invalid_argument("DcTable write() [Buffer]");

  const auto& lim = deviceVK().physLimits();

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
  if (ent->type == DcTypeUniform) {
    wr.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    if (offset % lim.minUniformBufferOffsetAlignment ||
        size > lim.maxUniformBufferRange)
      throw invalid_argument("DcTable write [Buffer] - limit");
  } else {
    wr.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    if (offset % lim.minStorageBufferOffsetAlignment ||
        size > lim.maxStorageBufferRange)
      throw invalid_argument("DcTable write [Buffer] - limit");
  }
  wr.pImageInfo = nullptr;
  wr.pBufferInfo = &info;
  wr.pTexelBufferView = nullptr;

  vkUpdateDescriptorSets(deviceVK().device(), 1, &wr, 0, nullptr);
}

void DcTableVK::write(uint32_t allocation, DcId id, uint32_t element,
                      Image& image, uint32_t layer, uint32_t level) {

  write(allocation, id, element, image, layer, level, nullptr);
}

void DcTableVK::write(uint32_t allocation, DcId id, uint32_t element,
                      Image& image, uint32_t layer, uint32_t level,
                      Sampler& sampler) {

  write(allocation, id, element, image, layer, level, &sampler);
}

void DcTableVK::write(uint32_t allocation, DcId id, uint32_t element,
                      Image& image, uint32_t layer, uint32_t level,
                      Sampler* sampler) {

  const auto ent = lower_bound(entries_.begin(), entries_.end(),
                               DcEntry{id, DcTypeImage, 0},
                               [](auto& a, auto& b) { return a.id < b.id; });

  if (allocation >= sets_.size() || ent == entries_.end() || ent->id != id ||
      (ent->type != DcTypeImage && ent->type != DcTypeImgSampler) ||
      element >= ent->elements || layer >= image.size().depthOrLayers)
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
  switch (ent->type) {
  case DcTypeImage:
    ref.sampler = nullptr;
    info.sampler = VK_NULL_HANDLE;
    wr.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    break;
  case DcTypeImgSampler:
    if (!ref.sampler || (sampler && *sampler != ref.sampler->sampler()))
      ref.sampler = make_unique<SamplerVK>(sampler ? *sampler : Sampler{});
    info.sampler = ref.sampler->handle();
    wr.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    break;
  default:
    break;
  }

  vkUpdateDescriptorSets(deviceVK().device(), 1, &wr, 0, nullptr);
}

const vector<DcEntry>& DcTableVK::entries() const {
  return entries_;
}

void DcTableVK::resetImgRefs() {
  imgRefs_.clear();

  // Cannot copy unique pointers
  for (auto i = sets_.size(); i > 0; i--) {
    imgRefs_.push_back({});

    for (const auto& e : entries_) {
      if (e.type != DcTypeImgSampler && e.type != DcTypeImage)
        continue;

      auto it = imgRefs_.back().emplace(e.id, vector<ImgRef>()).first;

      for (auto j = e.elements; j > 0; j--)
        it->second.push_back({nullptr, nullptr});
    }

    // TODO: Do this once
    if (imgRefs_[0].empty()) {
      imgRefs_.clear();
      return;
    }
  }
}

VkDescriptorSetLayout DcTableVK::dsLayout() {
  return dsLayout_;
}

VkDescriptorSet DcTableVK::ds(uint32_t index) {
  if (index >= sets_.size())
    throw invalid_argument("Descriptor set index out of range");

  return sets_[index];
}
