//
// CG
// BufferVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_BUFFERVK_H
#define YF_CG_BUFFERVK_H

#include "Buffer.h"
#include "VK.h"

CG_NS_BEGIN

class BufferVK final : public Buffer {
 public:
  BufferVK(uint64_t size);
  ~BufferVK();

  void write(uint64_t offset, uint64_t size, const void* data);

  VkBuffer handle() const;

 private:
  VkDeviceMemory _memory = VK_NULL_HANDLE;
  VkBuffer _handle = VK_NULL_HANDLE;
  void* _data;
};

CG_NS_END

#endif // YF_CG_BUFFERVK_H
