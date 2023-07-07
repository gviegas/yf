//
// CG
// BufferVK.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#ifndef YF_CG_BUFFERVK_H
#define YF_CG_BUFFERVK_H

#include "Buffer.h"
#include "VK.h"

CG_NS_BEGIN

class BufferVK final : public Buffer {
 public:
  BufferVK(const Desc& desc);
  ~BufferVK();

  void write(uint64_t offset, const void* data, uint64_t size);

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
