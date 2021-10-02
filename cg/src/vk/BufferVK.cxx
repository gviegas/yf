//
// CG
// BufferVK.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cstring>
#include <stdexcept>

#include "BufferVK.h"
#include "MemoryVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

BufferVK::BufferVK(uint64_t size, VkBufferUsageFlags usage) : size_(size) {
  if (size == 0)
    throw invalid_argument("BufferVK requires size > 0");

  // Create buffer
  auto dev = deviceVK().device();
  VkResult res;

  if (usage == 0) {
    usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            //VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT |
            //VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT |
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
            VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
  }

  VkBufferCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.size = size;
  info.usage = usage;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.queueFamilyIndexCount = 0;
  info.pQueueFamilyIndices = nullptr;

  res = vkCreateBuffer(dev, &info, nullptr, &handle_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create buffer");

  // Allocate/bind/map memory
  VkMemoryRequirements memReq;
  vkGetBufferMemoryRequirements(dev, handle_, &memReq);

  try {
    memory_ = allocateVK(memReq, true);
  } catch (...) {
    vkDestroyBuffer(dev, handle_, nullptr);
    throw;
  }

  res = vkBindBufferMemory(dev, handle_, memory_, 0);
  if (res != VK_SUCCESS) {
    vkDestroyBuffer(dev, handle_, nullptr);
    deallocateVK(memory_);
    throw DeviceExcept("Failed to bind memory to buffer");
  }

  res = vkMapMemory(dev, memory_, 0, VK_WHOLE_SIZE, 0, &data_);
  if (res != VK_SUCCESS) {
    vkDestroyBuffer(dev, handle_, nullptr);
    deallocateVK(memory_);
    throw DeviceExcept("Failed to map buffer memory");
  }
}

BufferVK::~BufferVK() {
  // TODO: Notify
  auto dev = deviceVK().device();
  vkDestroyBuffer(dev, handle_, nullptr);
  deallocateVK(memory_);
}

void BufferVK::write(uint64_t offset, uint64_t size, const void* data) {
  if (offset + size > size_ || !data)
    throw invalid_argument("Invalid BufferVK::write() argument(s)");

  memcpy(reinterpret_cast<char*>(data_)+offset, data, size);
}

uint64_t BufferVK::size() const {
  return size_;
}

VkBuffer BufferVK::handle() {
  return handle_;
}
