//
// CG
// ImageVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_IMAGEVK_H
#define YF_CG_IMAGEVK_H

#include "Image.h"
#include "VK.h"

CG_NS_BEGIN

class ImageVK final : Image {
 public:
  ImageVK(PxFormat format,
          Size2 size,
          uint32_t layers,
          uint32_t levels,
          Samples samples);

  ~ImageVK();

  void write(Offset2 offset,
             Size2 size,
             uint32_t layer,
             uint32_t level,
             const void* data);

  /// Getter.
  ///
  VkImage handle() const;

 private:
  VkDeviceMemory memory_ = VK_NULL_HANDLE;
  VkImage handle_ = VK_NULL_HANDLE;
  void* data_ = nullptr;
};

CG_NS_END

#endif // YF_CG_IMAGEVK_H
