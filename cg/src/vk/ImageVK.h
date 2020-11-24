//
// CG
// ImageVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_IMAGEVK_H
#define YF_CG_IMAGEVK_H

#include <unordered_map>

#include "Image.h"
#include "VK.h"

CG_NS_BEGIN

class ImageVK final : public Image {
 public:

  ImageVK(PxFormat format,
          Size2 size,
          uint32_t layers,
          uint32_t levels,
          Samples samples);

  /// Wraps an existing `VkImage` on a `ImageVK` object.
  ///
  ImageVK(PxFormat format,
          Size2 size,
          uint32_t layers,
          uint32_t levels,
          Samples samples,
          VkImageType type,
          VkImageTiling tiling,
          VkImageUsageFlags usage,
          VkImage handle,
          void* data,
          VkImageLayout layout,
          bool owned);

  ~ImageVK();

  void write(Offset2 offset,
             Size2 size,
             uint32_t layer,
             uint32_t level,
             const void* data);

  /// Performs a layout transition.
  ///
  void changeLayout(VkImageLayout newLayout, bool defer);

  /// Getter.
  ///
  VkImage handle() const;

  /// Image view.
  ///
  class View {
   public:
    using Ptr = std::unique_ptr<View>;
    View(ImageVK&, VkImageView, uint32_t, uint32_t, uint32_t, uint32_t);
    View(const View&) = delete;
    View& operator=(const View&) = delete;
    ~View();
    ImageVK& image() const;
    VkImageView handle() const;
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
  View::Ptr getView(uint32_t firstLayer,
                    uint32_t layerCount,
                    uint32_t firstLevel,
                    uint32_t levelCount);

 private:
  const bool owned_ = true;
  VkImageType type_ = VK_IMAGE_TYPE_2D;
  VkImageTiling tiling_ = VK_IMAGE_TILING_OPTIMAL;
  VkImageUsageFlags usage_ = 0;
  VkDeviceMemory memory_ = VK_NULL_HANDLE;
  VkImage handle_ = VK_NULL_HANDLE;
  void* data_ = nullptr;
  VkImageLayout layout_ = VK_IMAGE_LAYOUT_UNDEFINED;
  VkImageLayout nextLayout_ = VK_IMAGE_LAYOUT_UNDEFINED;
  //std::unordered_map<VkImageView, uint32_t> views_{};
};

/// Sampler.
///
class SamplerVK final {
 public:
  using Ptr = std::unique_ptr<SamplerVK>;
  SamplerVK(const SamplerVK&) = delete;
  SamplerVK& operator=(const SamplerVK&) = delete;
  ~SamplerVK();

  /// Getters.
  ///
  ImgSampler type() const;
  VkSampler handle() const;

  /// Makes a new sampler of a given type.
  ///
  static Ptr make(ImgSampler type);

 protected:
  SamplerVK(ImgSampler);

 private:
  ImgSampler type_ = ImgSamplerBasic;
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

  case PxFormatR32Uint: return VK_FORMAT_R32_UINT;
  case PxFormatR32Flt:  return VK_FORMAT_R32_SFLOAT;

  case PxFormatD16Unorm: return VK_FORMAT_D16_UNORM;
  case PxFormatD32Flt:   return VK_FORMAT_D32_SFLOAT;

  case PxFormatS8Uint: return VK_FORMAT_S8_UINT;

  case PxFormatD16UnormS8Uint: return VK_FORMAT_D16_UNORM_S8_UINT;
  case PxFormatD24UnormS8Uint: return VK_FORMAT_D24_UNORM_S8_UINT;
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
