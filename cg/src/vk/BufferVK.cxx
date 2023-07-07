//
// CG
// BufferVK.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#include <cstring>
#include <stdexcept>

#include "BufferVK.h"
#include "MemoryVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

// TODO: Should move `desc` validation to superclass.
BufferVK::BufferVK(const Desc& desc) : Buffer(desc) {
  if (size() == 0)
    throw invalid_argument("BufferVK requires size > 0");

  // Create buffer
  auto dev = deviceVK().device();
  VkResult res;

  VkBufferUsageFlags usage = 0;
  if (usageMask() & CopySrc)
    usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  if (usageMask() & (CopyDst | Query))
    usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  if (usageMask() & Vertex)
    usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  if (usageMask() & Index)
    usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  if (usageMask() & Indirect)
    usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
  if (usageMask() & Uniform)
    usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  if (usageMask() & Storage)
    usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

  VkBufferCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.size = size();
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
    memory_ = allocateVK(memReq, mode() == Shared);
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

  if (mode() == Shared) {
    // TODO: Consider exposing mapping/unmapping methods
    res = vkMapMemory(dev, memory_, 0, VK_WHOLE_SIZE, 0, &data_);
    if (res != VK_SUCCESS) {
      vkDestroyBuffer(dev, handle_, nullptr);
      deallocateVK(memory_);
      throw DeviceExcept("Failed to map buffer memory");
    }
  }
}

BufferVK::~BufferVK() {
  // TODO: Notify
  auto dev = deviceVK().device();
  vkDestroyBuffer(dev, handle_, nullptr);
  deallocateVK(memory_);
}

void BufferVK::write(uint64_t offset, const void* data, uint64_t size) {
  if (offset + size > this->size() || !data)
    throw invalid_argument("Invalid BufferVK::write() argument(s)");

  switch (mode()) {
  case Shared:
    memcpy(reinterpret_cast<char*>(data_)+offset, data, size);
    break;
  default:
    // TODO: Consider allowing this (do the write through a staging buffer)
    throw runtime_error("BufferVK::write() requires shared mode");
  }
}

VkBuffer BufferVK::handle() {
  return handle_;
}
