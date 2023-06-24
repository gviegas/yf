//
// CG
// ImageVK.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#ifndef YF_CG_IMAGEVK_H
#define YF_CG_IMAGEVK_H

#include <stdexcept>

#include "Image.h"
#include "VK.h"

CG_NS_BEGIN

class ImageVK final : public Image {
 public:
  ImageVK(const Desc& desc);

  /// Wraps an existing `VkImage`.
  ///
  /// If `owned` is set, then the destructor will call `vkDestroyImage`
  /// on `handle`.
  /// Non-null `data` indicates that the image uses linear tiling.
  ///
  ImageVK(const Desc& desc, VkImage handle, void* data, VkImageLayout layout,
          bool owned);

  ~ImageVK();

  ImageView::Ptr view(const ImageView::Desc& desc);

  void write(uint32_t plane, Origin3 origin, uint32_t level,
             const void* data, Size3 size, uint32_t bytesPerRow,
             uint32_t rowsPerSlice);

  Format format() const;
  Size3 size() const;
  uint32_t levels() const;
  Samples samples() const;
  Dimension dimension() const;
  UsageMask usageMask() const;

  /// Performs a layout transition.
  ///
  void changeLayout(VkImageLayout newLayout, bool defer);
  void changeLayout(const VkImageMemoryBarrier& barrier, bool defer);

  /// Notifies the image that it has transitioned to a new layout.
  ///
  // TODO: This should be atomic
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
  const Format format_{};
  const Size3 size_{0, 0, 0};
  const uint32_t levels_ = 0;
  const Samples samples_{};
  const Dimension dimension_{};
  const UsageMask usageMask_{};

  const bool owned_ = true;

  VkImageTiling tiling_ = VK_IMAGE_TILING_OPTIMAL;

  VkDeviceMemory memory_ = VK_NULL_HANDLE;
  VkImage handle_ = VK_NULL_HANDLE;
  void* data_ = nullptr;

  VkImageLayout layout_ = VK_IMAGE_LAYOUT_UNDEFINED;
  VkImageLayout nextLayout_ = VK_IMAGE_LAYOUT_UNDEFINED;
  VkImageMemoryBarrier barrier_{};

  void changeLayout(bool);
};

class ImageViewVK final : public ImageView {
 public:
  ImageViewVK(ImageVK& image, const ImageView::Desc& desc);
  ~ImageViewVK();

  Image& image();
  Range levels() const;
  Range layers() const;
  Dimension dimension() const;

 private:
  const Range levels_;
  const Range layers_;
  const Dimension dimension_;
  ImageVK* image_ = nullptr;
  VkImageView handle_ = VK_NULL_HANDLE;
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

/// Converts from a `Format` value.
///
inline VkFormat toFormatVK(Format format) {
  switch (format) {
  case Format::Undefined: return VK_FORMAT_UNDEFINED;

  case Format::R8Unorm: return VK_FORMAT_R8_UNORM;
  case Format::R8Norm:  return VK_FORMAT_R8_SNORM;
  case Format::R8Uint:  return VK_FORMAT_R8_UINT;
  case Format::R8Int:   return VK_FORMAT_R8_SINT;

  case Format::R16Uint:  return VK_FORMAT_R16_UINT;
  case Format::R16Int:   return VK_FORMAT_R16_SINT;
  case Format::R16Float: return VK_FORMAT_R16_SFLOAT;
  case Format::Rg8Unorm: return VK_FORMAT_R8G8_UNORM;
  case Format::Rg8Norm:  return VK_FORMAT_R8G8_SNORM;
  case Format::Rg8Uint:  return VK_FORMAT_R8G8_UINT;
  case Format::Rg8Int:   return VK_FORMAT_R8G8_SINT;

  case Format::R32Uint:      return VK_FORMAT_R32_UINT;
  case Format::R32Int:       return VK_FORMAT_R32_SINT;
  case Format::R32Float:     return VK_FORMAT_R32_SFLOAT;
  case Format::Rg16Uint:     return VK_FORMAT_R16G16_UINT;
  case Format::Rg16Int:      return VK_FORMAT_R16G16_SINT;
  case Format::Rg16Float:    return VK_FORMAT_R16G16_SFLOAT;
  case Format::Rgba8Unorm:   return VK_FORMAT_R8G8B8A8_UNORM;
  case Format::Rgba8Srgb:    return VK_FORMAT_R8G8B8A8_SRGB;
  case Format::Rgba8Norm:    return VK_FORMAT_R8G8B8A8_SNORM;
  case Format::Rgba8Uint:    return VK_FORMAT_R8G8B8A8_UINT;
  case Format::Rgba8Int:     return VK_FORMAT_R8G8B8A8_SINT;
  case Format::Bgra8Unorm:   return VK_FORMAT_B8G8R8A8_UNORM;
  case Format::Bgra8Srgb:    return VK_FORMAT_B8G8R8A8_SRGB;

  // TODO: These need remapping
  case Format::Rgb10a2Unorm: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
  case Format::Rg11b10Float: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;

  case Format::Rg32Uint:    return VK_FORMAT_R32G32_UINT;
  case Format::Rg32Int:     return VK_FORMAT_R32G32_SINT;
  case Format::Rg32Float:   return VK_FORMAT_R32G32_SFLOAT;
  case Format::Rgba16Uint:  return VK_FORMAT_R16G16B16A16_UINT;
  case Format::Rgba16Int:   return VK_FORMAT_R16G16B16A16_SINT;
  case Format::Rgba16Float: return VK_FORMAT_R16G16B16A16_SFLOAT;

  case Format::Rgba32Uint:  return VK_FORMAT_R32G32B32A32_UINT;
  case Format::Rgba32Int:   return VK_FORMAT_R32G32B32A32_SINT;
  case Format::Rgba32Float: return VK_FORMAT_R32G32B32A32_SFLOAT;

  case Format::D16Unorm:   return VK_FORMAT_D16_UNORM;
  case Format::D32Float:   return VK_FORMAT_D32_SFLOAT;
  case Format::S8:         return VK_FORMAT_S8_UINT;
  case Format::D24UnormS8: return VK_FORMAT_D24_UNORM_S8_UINT;
  case Format::D32FloatS8: return VK_FORMAT_D32_SFLOAT_S8_UINT;

  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts to a `Format` value.
///
inline Format fromFormatVK(VkFormat format) {
  switch (format) {
  case VK_FORMAT_R8_UNORM: return Format::R8Unorm;
  case VK_FORMAT_R8_SNORM: return Format::R8Norm;
  case VK_FORMAT_R8_UINT:  return Format::R8Uint;
  case VK_FORMAT_R8_SINT:  return Format::R8Int;

  case VK_FORMAT_R16_UINT:   return Format::R16Uint;
  case VK_FORMAT_R16_SINT:   return Format::R16Int;
  case VK_FORMAT_R16_SFLOAT: return Format::R16Float;
  case VK_FORMAT_R8G8_UNORM: return Format::Rg8Unorm;
  case VK_FORMAT_R8G8_SNORM: return Format::Rg8Norm;
  case VK_FORMAT_R8G8_UINT:  return Format::Rg8Uint;
  case VK_FORMAT_R8G8_SINT:  return Format::Rg8Int;

  case VK_FORMAT_R32_UINT:       return Format::R32Uint;
  case VK_FORMAT_R32_SINT:       return Format::R32Int;
  case VK_FORMAT_R32_SFLOAT:     return Format::R32Float;
  case VK_FORMAT_R16G16_UINT:    return Format::Rg16Uint;
  case VK_FORMAT_R16G16_SINT:    return Format::Rg16Int;
  case VK_FORMAT_R16G16_SFLOAT:  return Format::Rg16Float;
  case VK_FORMAT_R8G8B8A8_UNORM: return Format::Rgba8Unorm;
  case VK_FORMAT_R8G8B8A8_SRGB:  return Format::Rgba8Srgb;
  case VK_FORMAT_R8G8B8A8_SNORM: return Format::Rgba8Norm;
  case VK_FORMAT_R8G8B8A8_UINT:  return Format::Rgba8Uint;
  case VK_FORMAT_R8G8B8A8_SINT:  return Format::Rgba8Int;
  case VK_FORMAT_B8G8R8A8_UNORM: return Format::Bgra8Unorm;
  case VK_FORMAT_B8G8R8A8_SRGB:  return Format::Bgra8Srgb;

  // TODO: These need remapping
  case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return Format::Rgb10a2Unorm;
  case VK_FORMAT_B10G11R11_UFLOAT_PACK32:  return Format::Rg11b10Float;

  case VK_FORMAT_R32G32_UINT:         return Format::Rg32Uint;
  case VK_FORMAT_R32G32_SINT:         return Format::Rg32Int;
  case VK_FORMAT_R16G16B16A16_UINT:   return Format::Rgba16Uint;
  case VK_FORMAT_R16G16B16A16_SINT:   return Format::Rgba16Int;
  case VK_FORMAT_R16G16B16A16_SFLOAT: return Format::Rgba16Float;

  case VK_FORMAT_R32G32B32A32_UINT:   return Format::Rgba32Uint;
  case VK_FORMAT_R32G32B32A32_SINT:   return Format::Rgba32Int;
  case VK_FORMAT_R32G32B32A32_SFLOAT: return Format::Rgba32Float;

  case VK_FORMAT_D16_UNORM:          return Format::D16Unorm;
  case VK_FORMAT_D32_SFLOAT:         return Format::D32Float;
  case VK_FORMAT_S8_UINT:            return Format::S8;
  case VK_FORMAT_D24_UNORM_S8_UINT:  return Format::D24UnormS8;
  case VK_FORMAT_D32_SFLOAT_S8_UINT: return Format::D32FloatS8;

  case VK_FORMAT_UNDEFINED:
  default:
    return Format::Undefined;
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

/// Gets the aspect of a given `Format` value.
///
inline VkImageAspectFlags aspectOfVK(Format format) {
  switch (format) {
  case Format::Undefined:
    return 0;

  case Format::R8Unorm:
  case Format::R8Norm:
  case Format::R8Uint:
  case Format::R8Int:
  case Format::R16Uint:
  case Format::R16Int:
  case Format::R16Float:
  case Format::Rg8Unorm:
  case Format::Rg8Norm:
  case Format::Rg8Uint:
  case Format::Rg8Int:
  case Format::R32Uint:
  case Format::R32Int:
  case Format::R32Float:
  case Format::Rg16Uint:
  case Format::Rg16Int:
  case Format::Rg16Float:
  case Format::Rgba8Unorm:
  case Format::Rgba8Srgb:
  case Format::Rgba8Norm:
  case Format::Rgba8Uint:
  case Format::Rgba8Int:
  case Format::Bgra8Unorm:
  case Format::Bgra8Srgb:
  case Format::Rgb10a2Unorm:
  case Format::Rg11b10Float:
  case Format::Rg32Uint:
  case Format::Rg32Int:
  case Format::Rg32Float:
  case Format::Rgba16Uint:
  case Format::Rgba16Int:
  case Format::Rgba16Float:
  case Format::Rgba32Uint:
  case Format::Rgba32Int:
  case Format::Rgba32Float:
    return VK_IMAGE_ASPECT_COLOR_BIT;

  case Format::D16Unorm:
  case Format::D32Float:
    return VK_IMAGE_ASPECT_DEPTH_BIT;

  case Format::S8:
    return VK_IMAGE_ASPECT_STENCIL_BIT;

  case Format::D24UnormS8:
  case Format::D32FloatS8:
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
