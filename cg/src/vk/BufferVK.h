//
// CG
// BufferVK.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_BUFFERVK_H
#define YF_CG_BUFFERVK_H

#include "Buffer.h"
#include "VK.h"

CG_NS_BEGIN

class BufferVK final : public Buffer {
 public:
  BufferVK(uint64_t size, VkBufferUsageFlags usage = 0);
  ~BufferVK();

  void write(uint64_t offset, uint64_t size, const void* data);

  /// Getter.
  ///
  VkBuffer handle();

 private:
  VkDeviceMemory memory_ = VK_NULL_HANDLE;
  VkBuffer handle_ = VK_NULL_HANDLE;
  void* data_ = nullptr;
};

CG_NS_END

#endif // YF_CG_BUFFERVK_H
