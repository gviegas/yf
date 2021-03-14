//
// CG
// MemoryVK.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <stdexcept>

#include "MemoryVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

/// Selects a suitable memory heap.
///
int32_t selectMemory(uint32_t requirement, VkMemoryPropertyFlags properties) {
  const auto& memProp = deviceVK().memProperties();
  for (uint32_t i = 0; i < memProp.memoryTypeCount; ++i) {
    if (requirement & (1 << i)) {
      auto propFlags = memProp.memoryTypes[i].propertyFlags;
      if ((propFlags & properties) == properties)
        return i;
    }
  }
  return -1;
}

INTERNAL_NS_END

VkDeviceMemory CG_NS::allocateVK(const VkMemoryRequirements& requirements,
                                 bool hostVisible) {

  VkMemoryPropertyFlags prop = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  int32_t memType = -1;

  if (hostVisible)
    prop |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  memType = selectMemory(requirements.memoryTypeBits, prop);
  if (memType == -1) {
    prop &= ~VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    memType = selectMemory(requirements.memoryTypeBits, prop);
    if (memType == -1)
      throw UnsupportedExcept("Failed to find a suitable memory type");
  }

  VkMemoryAllocateInfo info;
  info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  info.pNext = nullptr;
  info.allocationSize = requirements.size;
  info.memoryTypeIndex = memType;

  VkDeviceMemory mem;
  auto dev = deviceVK().device();
  auto res = vkAllocateMemory(dev, &info, nullptr, &mem);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Failed to allocate device memory");

  return mem;
}

void CG_NS::deallocateVK(VkDeviceMemory memory) {
  auto dev = deviceVK().device();
  vkFreeMemory(dev, memory, nullptr);
}
