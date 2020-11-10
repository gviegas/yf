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
  VkImageType type_ = VK_IMAGE_TYPE_2D;
  VkImageTiling tiling_ = VK_IMAGE_TILING_OPTIMAL;
  VkImageUsageFlags usage_ = 0;
  VkDeviceMemory memory_ = VK_NULL_HANDLE;
  VkImage handle_ = VK_NULL_HANDLE;
  void* data_ = nullptr;
  VkImageLayout layout_ = VK_IMAGE_LAYOUT_UNDEFINED;
};

/// Converts from a `PxFormat` value.
///
inline VkFormat toFormatVK(PxFormat pxFormat) {
  switch (pxFormat) {
  case PxFormatUndefined:  return VK_FORMAT_UNDEFINED;
  case PxFormatBgra8Srgb:  return VK_FORMAT_B8G8R8A8_SRGB;
  case PxFormatRgba8Unorm: return VK_FORMAT_R8G8B8A8_UNORM;
  case PxFormatD16Unorm:   return VK_FORMAT_D16_UNORM;
  }
}

/// Converts from a `Samples` value.
///
inline VkSampleCountFlagBits toSampleCountVK(Samples samples) {
  switch (samples) {
  case Samples1:  return VK_SAMPLE_COUNT_1_BIT;
  case Samples2:  return VK_SAMPLE_COUNT_2_BIT;
  case Samples4:  return VK_SAMPLE_COUNT_4_BIT;
  case Samples8:  return VK_SAMPLE_COUNT_8_BIT;
  case Samples16: return VK_SAMPLE_COUNT_16_BIT;
  case Samples32: return VK_SAMPLE_COUNT_32_BIT;
  case Samples64: return VK_SAMPLE_COUNT_64_BIT;
  }
}

/// Gets the aspect from a given `PxFormat` value.
///
inline VkImageAspectFlags aspectOfVK(PxFormat pxFormat) {
  switch (pxFormat) {
  case PxFormatUndefined:
    return 0;
  case PxFormatBgra8Srgb:
  case PxFormatRgba8Unorm:
    return VK_IMAGE_ASPECT_COLOR_BIT;
  case PxFormatD16Unorm:
    return VK_IMAGE_ASPECT_DEPTH_BIT;
  }
}

CG_NS_END

#endif // YF_CG_IMAGEVK_H
