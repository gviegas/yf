//
// CG
// ImageVK.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
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

  PxFormat format() const;
  Size2 size() const;
  uint32_t layers() const;
  uint32_t levels() const;
  Samples samples() const;

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
  const PxFormat format_{};
  const Size2 size_{0, 0};
  const uint32_t layers_ = 0;
  const uint32_t levels_ = 0;
  const Samples samples_{};

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
  case PxFormatR8Norm:  return VK_FORMAT_R8_SNORM;
  case PxFormatR8Uint:  return VK_FORMAT_R8_UINT;
  case PxFormatR8Int:   return VK_FORMAT_R8_SINT;

  case PxFormatR16Uint:  return VK_FORMAT_R16_UINT;
  case PxFormatR16Int:   return VK_FORMAT_R16_SINT;
  case PxFormatR16Float: return VK_FORMAT_R16_SFLOAT;
  case PxFormatRg8Unorm: return VK_FORMAT_R8G8_UNORM;
  case PxFormatRg8Norm:  return VK_FORMAT_R8G8_SNORM;
  case PxFormatRg8Uint:  return VK_FORMAT_R8G8_UINT;
  case PxFormatRg8Int:   return VK_FORMAT_R8G8_SINT;

  case PxFormatR32Uint:      return VK_FORMAT_R32_UINT;
  case PxFormatR32Int:       return VK_FORMAT_R32_SINT;
  case PxFormatR32Float:     return VK_FORMAT_R32_SFLOAT;
  case PxFormatRg16Uint:     return VK_FORMAT_R16G16_UINT;
  case PxFormatRg16Int:      return VK_FORMAT_R16G16_SINT;
  case PxFormatRg16Float:    return VK_FORMAT_R16G16_SFLOAT;
  case PxFormatRgba8Unorm:   return VK_FORMAT_R8G8B8A8_UNORM;
  case PxFormatRgba8Srgb:    return VK_FORMAT_R8G8B8A8_SRGB;
  case PxFormatRgba8Norm:    return VK_FORMAT_R8G8B8A8_SNORM;
  case PxFormatRgba8Uint:    return VK_FORMAT_R8G8B8A8_UINT;
  case PxFormatRgba8Int:     return VK_FORMAT_R8G8B8A8_SINT;
  case PxFormatBgra8Unorm:   return VK_FORMAT_B8G8R8A8_UNORM;
  case PxFormatBgra8Srgb:    return VK_FORMAT_B8G8R8A8_SRGB;

  // TODO: These need remapping
  case PxFormatRgb10a2Unorm: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
  case PxFormatRg11b10Float: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;

  case PxFormatRg32Uint:    return VK_FORMAT_R32G32_UINT;
  case PxFormatRg32Int:     return VK_FORMAT_R32G32_SINT;
  case PxFormatRg32Float:   return VK_FORMAT_R32G32_SFLOAT;
  case PxFormatRgba16Uint:  return VK_FORMAT_R16G16B16A16_UINT;
  case PxFormatRgba16Int:   return VK_FORMAT_R16G16B16A16_SINT;
  case PxFormatRgba16Float: return VK_FORMAT_R16G16B16A16_SFLOAT;

  case PxFormatRgba32Uint:  return VK_FORMAT_R32G32B32A32_UINT;
  case PxFormatRgba32Int:   return VK_FORMAT_R32G32B32A32_SINT;
  case PxFormatRgba32Float: return VK_FORMAT_R32G32B32A32_SFLOAT;

  case PxFormatD16Unorm:   return VK_FORMAT_D16_UNORM;
  case PxFormatD32Float:   return VK_FORMAT_D32_SFLOAT;
  case PxFormatS8:         return VK_FORMAT_S8_UINT;
  case PxFormatD24UnormS8: return VK_FORMAT_D24_UNORM_S8_UINT;
  case PxFormatD32FloatS8: return VK_FORMAT_D32_SFLOAT_S8_UINT;

  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts to a `PxFormat` value.
///
inline PxFormat fromFormatVK(VkFormat format) {
  switch (format) {
  case VK_FORMAT_R8_UNORM: return PxFormatR8Unorm;
  case VK_FORMAT_R8_SNORM: return PxFormatR8Norm;
  case VK_FORMAT_R8_UINT:  return PxFormatR8Uint;
  case VK_FORMAT_R8_SINT:  return PxFormatR8Int;

  case VK_FORMAT_R16_UINT:   return PxFormatR16Uint;
  case VK_FORMAT_R16_SINT:   return PxFormatR16Int;
  case VK_FORMAT_R16_SFLOAT: return PxFormatR16Float;
  case VK_FORMAT_R8G8_UNORM: return PxFormatRg8Unorm;
  case VK_FORMAT_R8G8_SNORM: return PxFormatRg8Norm;
  case VK_FORMAT_R8G8_UINT:  return PxFormatRg8Uint;
  case VK_FORMAT_R8G8_SINT:  return PxFormatRg8Int;

  case VK_FORMAT_R32_UINT:       return PxFormatR32Uint;
  case VK_FORMAT_R32_SINT:       return PxFormatR32Int;
  case VK_FORMAT_R32_SFLOAT:     return PxFormatR32Float;
  case VK_FORMAT_R16G16_UINT:    return PxFormatRg16Uint;
  case VK_FORMAT_R16G16_SINT:    return PxFormatRg16Int;
  case VK_FORMAT_R16G16_SFLOAT:  return PxFormatRg16Float;
  case VK_FORMAT_R8G8B8A8_UNORM: return PxFormatRgba8Unorm;
  case VK_FORMAT_R8G8B8A8_SRGB:  return PxFormatRgba8Srgb;
  case VK_FORMAT_R8G8B8A8_SNORM: return PxFormatRgba8Norm;
  case VK_FORMAT_R8G8B8A8_UINT:  return PxFormatRgba8Uint;
  case VK_FORMAT_R8G8B8A8_SINT:  return PxFormatRgba8Int;
  case VK_FORMAT_B8G8R8A8_UNORM: return PxFormatBgra8Unorm;
  case VK_FORMAT_B8G8R8A8_SRGB:  return PxFormatBgra8Srgb;

  // TODO: These need remapping
  case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return PxFormatRgb10a2Unorm;
  case VK_FORMAT_B10G11R11_UFLOAT_PACK32:  return PxFormatRg11b10Float;

  case VK_FORMAT_R32G32_UINT:         return PxFormatRg32Uint;
  case VK_FORMAT_R32G32_SINT:         return PxFormatRg32Int;
  case VK_FORMAT_R16G16B16A16_UINT:   return PxFormatRgba16Uint;
  case VK_FORMAT_R16G16B16A16_SINT:   return PxFormatRgba16Int;
  case VK_FORMAT_R16G16B16A16_SFLOAT: return PxFormatRgba16Float;

  case VK_FORMAT_R32G32B32A32_UINT:   return PxFormatRgba32Uint;
  case VK_FORMAT_R32G32B32A32_SINT:   return PxFormatRgba32Int;
  case VK_FORMAT_R32G32B32A32_SFLOAT: return PxFormatRgba32Float;

  case VK_FORMAT_D16_UNORM:          return PxFormatD16Unorm;
  case VK_FORMAT_D32_SFLOAT:         return PxFormatD32Float;
  case VK_FORMAT_S8_UINT:            return PxFormatS8;
  case VK_FORMAT_D24_UNORM_S8_UINT:  return PxFormatD24UnormS8;
  case VK_FORMAT_D32_SFLOAT_S8_UINT: return PxFormatD32FloatS8;

  case VK_FORMAT_UNDEFINED:
  default:
    return PxFormatUndefined;
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
  case PxFormatR8Norm:
  case PxFormatR8Uint:
  case PxFormatR8Int:
  case PxFormatR16Uint:
  case PxFormatR16Int:
  case PxFormatR16Float:
  case PxFormatRg8Unorm:
  case PxFormatRg8Norm:
  case PxFormatRg8Uint:
  case PxFormatRg8Int:
  case PxFormatR32Uint:
  case PxFormatR32Int:
  case PxFormatR32Float:
  case PxFormatRg16Uint:
  case PxFormatRg16Int:
  case PxFormatRg16Float:
  case PxFormatRgba8Unorm:
  case PxFormatRgba8Srgb:
  case PxFormatRgba8Norm:
  case PxFormatRgba8Uint:
  case PxFormatRgba8Int:
  case PxFormatBgra8Unorm:
  case PxFormatBgra8Srgb:
  case PxFormatRgb10a2Unorm:
  case PxFormatRg11b10Float:
  case PxFormatRg32Uint:
  case PxFormatRg32Int:
  case PxFormatRg32Float:
  case PxFormatRgba16Uint:
  case PxFormatRgba16Int:
  case PxFormatRgba16Float:
  case PxFormatRgba32Uint:
  case PxFormatRgba32Int:
  case PxFormatRgba32Float:
    return VK_IMAGE_ASPECT_COLOR_BIT;

  case PxFormatD16Unorm:
  case PxFormatD32Float:
    return VK_IMAGE_ASPECT_DEPTH_BIT;

  case PxFormatS8:
    return VK_IMAGE_ASPECT_STENCIL_BIT;

  case PxFormatD24UnormS8:
  case PxFormatD32FloatS8:
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
