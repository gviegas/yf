//
// CG
// ImageVK.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_IMAGEVK_H
#define YF_CG_IMAGEVK_H

#include <unordered_map>
#include <stdexcept>

#include "Image.h"
#include "VK.h"
#include "BufferVK.h"

CG_NS_BEGIN

class ImageVK final : public Image {
 public:
  ImageVK(PxFormat format, Size2 size, uint32_t layers, uint32_t levels,
          Samples samples);

  /// Wraps an existing `VkImage` on an `ImageVK` object.
  ///
  ImageVK(PxFormat format, Size2 size, uint32_t layers, uint32_t levels,
          Samples samples, VkImageType type, VkImageTiling tiling,
          VkImageUsageFlags usage, VkImage handle, void* data,
          VkImageLayout layout, bool owned);

  ~ImageVK();

  void write(Offset2 offset, Size2 size, uint32_t layer, uint32_t level,
             const void* data);

  /// Performs a layout transition.
  ///
  void changeLayout(VkImageLayout newLayout, bool defer);
  void changeLayout(const VkImageMemoryBarrier& barrier, bool defer);

  /// Notifies the image that it has transitioned to a new layout.
  ///
  void layoutChanged(VkImageLayout newLayout);

  /// Getters.
  ///
  VkImage handle();
  std::pair<VkImageLayout, VkImageLayout> layout() const;

  /// Image view.
  ///
  class View {
   public:
    using Ptr = std::unique_ptr<View>;

    View(ImageVK&, VkImageView, uint32_t, uint32_t, uint32_t, uint32_t);
    View(const View&) = delete;
    View& operator=(const View&) = delete;
    ~View();

    /// Getters.
    ///
    ImageVK& image();
    VkImageView handle();
    uint32_t firstLayer() const;
    uint32_t layerCount() const;
    uint32_t firstLevel() const;
    uint32_t levelCount() const;

   private:
    ImageVK& image_;
    VkImageView handle_;
    uint32_t firstLayer_;
    uint32_t layerCount_;
    uint32_t firstLevel_;
    uint32_t levelCount_;
  };

  /// Gets an image view.
  ///
  View::Ptr getView(uint32_t firstLayer, uint32_t layerCount,
                    uint32_t firstLevel, uint32_t levelCount);

 private:
  const bool owned_ = true;

  VkImageType type_ = VK_IMAGE_TYPE_2D;
  VkImageTiling tiling_ = VK_IMAGE_TILING_OPTIMAL;
  VkImageUsageFlags usage_ = 0;

  VkDeviceMemory memory_ = VK_NULL_HANDLE;
  VkImage handle_ = VK_NULL_HANDLE;
  void* data_ = nullptr;
  std::unordered_map<uint32_t, std::unique_ptr<BufferVK>> staging_{};

  VkImageLayout layout_ = VK_IMAGE_LAYOUT_UNDEFINED;
  VkImageLayout nextLayout_ = VK_IMAGE_LAYOUT_UNDEFINED;
  VkImageMemoryBarrier barrier_{};

  void changeLayout(bool);
};

/// Image sampler.
///
class SamplerVK final {
 public:
  using Ptr = std::unique_ptr<SamplerVK>;

  SamplerVK(const Sampler& sampler);
  SamplerVK(const SamplerVK&) = delete;
  SamplerVK& operator=(const SamplerVK&) = delete;
  ~SamplerVK();

  /// Getters.
  ///
  const Sampler& sampler() const;
  VkSampler handle();

 private:
  Sampler sampler_{};
  VkSampler handle_ = VK_NULL_HANDLE;
};

/// Converts from a `PxFormat` value.
///
inline VkFormat toFormatVK(PxFormat pxFormat) {
  switch (pxFormat) {
  case PxFormatUndefined: return VK_FORMAT_UNDEFINED;

  case PxFormatR8Unorm: return VK_FORMAT_R8_UNORM;
  case PxFormatR8Uint:  return VK_FORMAT_R8_UINT;
  case PxFormatR8Srgb:  return VK_FORMAT_R8_SRGB;

  case PxFormatRg8Unorm: return VK_FORMAT_R8G8_UNORM;
  case PxFormatRg8Uint:  return VK_FORMAT_R8G8_UINT;
  case PxFormatRg8Srgb:  return VK_FORMAT_R8G8_SRGB;

  case PxFormatRgb8Unorm: return VK_FORMAT_R8G8B8_UNORM;
  case PxFormatRgb8Uint:  return VK_FORMAT_R8G8B8_UINT;
  case PxFormatRgb8Srgb:  return VK_FORMAT_R8G8B8_SRGB;
  case PxFormatBgr8Unorm: return VK_FORMAT_B8G8R8_UNORM;
  case PxFormatBgr8Uint:  return VK_FORMAT_B8G8R8_UINT;
  case PxFormatBgr8Srgb:  return VK_FORMAT_B8G8R8_SRGB;

  case PxFormatRgba8Unorm: return VK_FORMAT_R8G8B8A8_UNORM;
  case PxFormatRgba8Uint:  return VK_FORMAT_R8G8B8A8_UINT;
  case PxFormatRgba8Srgb:  return VK_FORMAT_R8G8B8A8_SRGB;
  case PxFormatBgra8Unorm: return VK_FORMAT_B8G8R8A8_UNORM;
  case PxFormatBgra8Uint:  return VK_FORMAT_B8G8R8A8_UINT;
  case PxFormatBgra8Srgb:  return VK_FORMAT_B8G8R8A8_SRGB;

  case PxFormatR16Unorm: return VK_FORMAT_R16_UNORM;
  case PxFormatR16Uint:  return VK_FORMAT_R16_UINT;

  case PxFormatRg16Unorm: return VK_FORMAT_R16G16_UNORM;
  case PxFormatRg16Uint:  return VK_FORMAT_R16G16_UINT;

  case PxFormatRgb16Unorm: return VK_FORMAT_R16G16B16_UNORM;
  case PxFormatRgb16Uint:  return VK_FORMAT_R16G16B16_UINT;

  case PxFormatRgba16Unorm: return VK_FORMAT_R16G16B16A16_UNORM;
  case PxFormatRgba16Uint:  return VK_FORMAT_R16G16B16A16_UINT;

  case PxFormatR32Uint: return VK_FORMAT_R32_UINT;
  case PxFormatR32Flt:  return VK_FORMAT_R32_SFLOAT;

  case PxFormatD16Unorm: return VK_FORMAT_D16_UNORM;
  case PxFormatD32Flt:   return VK_FORMAT_D32_SFLOAT;

  case PxFormatS8Uint: return VK_FORMAT_S8_UINT;

  case PxFormatD16UnormS8Uint: return VK_FORMAT_D16_UNORM_S8_UINT;
  case PxFormatD24UnormS8Uint: return VK_FORMAT_D24_UNORM_S8_UINT;

  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts to a `PxFormat` value.
///
inline PxFormat fromFormatVK(VkFormat format) {
  switch (format) {
  case VK_FORMAT_R8_UNORM: return PxFormatR8Unorm;
  case VK_FORMAT_R8_UINT:  return PxFormatR8Uint;
  case VK_FORMAT_R8_SRGB:  return PxFormatR8Srgb;

  case VK_FORMAT_R8G8_UNORM: return PxFormatRg8Unorm;
  case VK_FORMAT_R8G8_UINT:  return PxFormatRg8Uint;
  case VK_FORMAT_R8G8_SRGB:  return PxFormatRg8Srgb;

  case VK_FORMAT_R8G8B8_UNORM: return PxFormatRgb8Unorm;
  case VK_FORMAT_R8G8B8_UINT:  return PxFormatRgb8Uint;
  case VK_FORMAT_R8G8B8_SRGB:  return PxFormatRgb8Srgb;
  case VK_FORMAT_B8G8R8_UNORM: return PxFormatBgr8Unorm;
  case VK_FORMAT_B8G8R8_UINT:  return PxFormatBgr8Uint;
  case VK_FORMAT_B8G8R8_SRGB:  return PxFormatBgr8Srgb;

  case VK_FORMAT_R8G8B8A8_UNORM: return PxFormatRgba8Unorm;
  case VK_FORMAT_R8G8B8A8_UINT:  return PxFormatRgba8Uint;
  case VK_FORMAT_R8G8B8A8_SRGB:  return PxFormatRgba8Srgb;
  case VK_FORMAT_B8G8R8A8_UNORM: return PxFormatBgra8Unorm;
  case VK_FORMAT_B8G8R8A8_UINT:  return PxFormatBgra8Uint;
  case VK_FORMAT_B8G8R8A8_SRGB:  return PxFormatBgra8Srgb;

  case VK_FORMAT_R16_UNORM: return PxFormatR16Unorm;
  case VK_FORMAT_R16_UINT:  return PxFormatR16Uint;

  case VK_FORMAT_R16G16_UNORM: return PxFormatRg16Unorm;
  case VK_FORMAT_R16G16_UINT:  return PxFormatRg16Uint;

  case VK_FORMAT_R16G16B16_UNORM: return PxFormatRgb16Unorm;
  case VK_FORMAT_R16G16B16_UINT:  return PxFormatRgb16Uint;

  case VK_FORMAT_R16G16B16A16_UNORM: return PxFormatRgba16Unorm;
  case VK_FORMAT_R16G16B16A16_UINT:  return PxFormatRgba16Uint;

  case VK_FORMAT_R32_UINT:   return PxFormatR32Uint;
  case VK_FORMAT_R32_SFLOAT: return PxFormatR32Flt;

  case VK_FORMAT_D16_UNORM:  return PxFormatD16Unorm;
  case VK_FORMAT_D32_SFLOAT: return PxFormatD32Flt;

  case VK_FORMAT_S8_UINT: return PxFormatS8Uint;

  case VK_FORMAT_D16_UNORM_S8_UINT: return PxFormatD16UnormS8Uint;
  case VK_FORMAT_D24_UNORM_S8_UINT: return PxFormatD24UnormS8Uint;

  default: return PxFormatUndefined;
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
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Gets the aspect from a given `PxFormat` value.
///
inline VkImageAspectFlags aspectOfVK(PxFormat pxFormat) {
  switch (pxFormat) {
  case PxFormatUndefined:
    return 0;

  case PxFormatR8Unorm:
  case PxFormatR8Uint:
  case PxFormatR8Srgb:
  case PxFormatRg8Unorm:
  case PxFormatRg8Uint:
  case PxFormatRg8Srgb:
  case PxFormatRgb8Unorm:
  case PxFormatRgb8Uint:
  case PxFormatRgb8Srgb:
  case PxFormatBgr8Unorm:
  case PxFormatBgr8Uint:
  case PxFormatBgr8Srgb:
  case PxFormatRgba8Unorm:
  case PxFormatRgba8Uint:
  case PxFormatRgba8Srgb:
  case PxFormatBgra8Unorm:
  case PxFormatBgra8Uint:
  case PxFormatBgra8Srgb:
  case PxFormatR16Unorm:
  case PxFormatR16Uint:
  case PxFormatRg16Unorm:
  case PxFormatRg16Uint:
  case PxFormatRgb16Unorm:
  case PxFormatRgb16Uint:
  case PxFormatRgba16Unorm:
  case PxFormatRgba16Uint:
  case PxFormatR32Uint:
  case PxFormatR32Flt:
    return VK_IMAGE_ASPECT_COLOR_BIT;

  case PxFormatD16Unorm:
  case PxFormatD32Flt:
    return VK_IMAGE_ASPECT_DEPTH_BIT;

  case PxFormatS8Uint:
    return VK_IMAGE_ASPECT_STENCIL_BIT;

  case PxFormatD16UnormS8Uint:
  case PxFormatD24UnormS8Uint:
    return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `WrapMode` value.
///
inline VkSamplerAddressMode toAddressModeVK(WrapMode wrapMode) {
  switch (wrapMode) {
  case WrapModeClamp:  return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  case WrapModeMirror: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
  case WrapModeRepeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `Filter` value (magnification).
///
inline VkFilter toFilterVK(Filter magFilter) {
  switch (magFilter) {
  case FilterNearest: return VK_FILTER_NEAREST;
  case FilterLinear:  return VK_FILTER_LINEAR;
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `Filter` value (minification).
///
inline VkFilter toFilterVK(Filter minFilter, VkSamplerMipmapMode& mipmapMode,
                           float& minLod, float& maxLod) {

  switch (minFilter) {
  case FilterNearest:
    mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    minLod = 0.0f;
    maxLod = 0.25f;
    return VK_FILTER_NEAREST;

  case FilterLinear:
    mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    minLod = 0.0f;
    maxLod = 0.25f;
    return VK_FILTER_LINEAR;

  case FilterNearestNearest:
    mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    minLod = 0.0f;
    maxLod = VK_LOD_CLAMP_NONE;
    return VK_FILTER_NEAREST;

  case FilterNearestLinear:
    mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    minLod = 0.0f;
    maxLod = VK_LOD_CLAMP_NONE;
    return VK_FILTER_NEAREST;

  case FilterLinearNearest:
    mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    minLod = 0.0f;
    maxLod = VK_LOD_CLAMP_NONE;
    return VK_FILTER_LINEAR;

  case FilterLinearLinear:
    mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    minLod = 0.0f;
    maxLod = VK_LOD_CLAMP_NONE;
    return VK_FILTER_LINEAR;

  default:
    throw std::invalid_argument(__func__);
  }
}

CG_NS_END

#endif // YF_CG_IMAGEVK_H
