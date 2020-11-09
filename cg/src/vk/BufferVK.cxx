//
// cg
// BufferVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <stdexcept>
#include <cstring>

#include "BufferVK.h"
#include "DeviceVK.h"
#include "MemoryVK.h"

using namespace CG_NS;
using namespace std;

BufferVK::BufferVK(uint64_t size) : Buffer(size) {
  if (size == 0)
    // TODO
    throw invalid_argument("BufferVK requires size > 0");

  auto dev = DeviceVK::get().device();
  VkResult res;

  VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                             //VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT |
                             //VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT |
                             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
                             VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                             VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

  VkBufferCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.size = size;
  info.usage = usage;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.queueFamilyIndexCount = 0;
  info.pQueueFamilyIndices = nullptr;

  res = vkCreateBuffer(dev, &info, nullptr, &_handle);
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Could not create buffer");

  VkMemoryRequirements memReq;
  vkGetBufferMemoryRequirements(dev, _handle, &memReq);
  _memory = allocateVK(memReq, true);

  res = vkBindBufferMemory(dev, _handle, _memory, 0);
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Failed to bind memory to buffer");

  res = vkMapMemory(dev, _memory, 0, VK_WHOLE_SIZE, 0, &_data);
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Failed to map buffer memory");
}

BufferVK::~BufferVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyBuffer(dev, _handle, nullptr);
  deallocateVK(_memory);
}

Result BufferVK::write(uint64_t offset, uint64_t size, const void* data) {
  if (offset + size > this->size || !data)
    // TODO
    throw invalid_argument("Invalid BufferVK::write() argument(s)");

  memcpy(reinterpret_cast<uint8_t*>(_data)+offset, data, size);
  return Result::Success;
}

VkBuffer BufferVK::handle() const {
  return _handle;
}
