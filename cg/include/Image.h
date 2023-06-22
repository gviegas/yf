//
// CG
// Image.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#ifndef YF_CG_IMAGE_H
#define YF_CG_IMAGE_H

#include <cstdint>
#include <memory>

#include "yf/cg/Defs.h"
#include "yf/cg/Types.h"

CG_NS_BEGIN

/// Pixel formats.
///
enum class Format {
  Undefined,

  // 8-bit
  R8Unorm,
  R8Norm,
  R8Uint,
  R8Int,

  // 16-bit
  R16Uint,
  R16Int,
  R16Float,
  Rg8Unorm,
  Rg8Norm,
  Rg8Uint,
  Rg8Int,

  // 32-bit
  R32Uint,
  R32Int,
  R32Float,
  Rg16Uint,
  Rg16Int,
  Rg16Float,
  Rgba8Unorm,
  Rgba8Srgb,
  Rgba8Norm,
  Rgba8Uint,
  Rgba8Int,
  Bgra8Unorm,
  Bgra8Srgb,
  Rgb10a2Unorm,
  Rg11b10Float,

  // 64-bit
  Rg32Uint,
  Rg32Int,
  Rg32Float,
  Rgba16Uint,
  Rgba16Int,
  Rgba16Float,

  // 128-bit
  Rgba32Uint,
  Rgba32Int,
  Rgba32Float,

  // Depth/stencil
  D16Unorm,
  D32Float,
  S8,
  D24UnormS8,
  D32FloatS8
};

/// Sample counts.
///
enum Samples {
  Samples1,
  Samples2,
  Samples4,
  Samples8,
  Samples16,
  Samples32,
  Samples64
};

class Image;

/// View onto an image subresource.
///
class ImageView {
 public:
  using Ptr = std::unique_ptr<ImageView>;

  /// View dimensionality.
  ///
  enum class Dimension {
    Dim1,
    Dim1Array,
    Dim2,
    Dim2Array,
    DimCube,
    DimCubeArray,
    Dim3,
  };
  // TODO: Update this when migrating to C++20
#if __cplusplus >= 202002L
# error Use `using` instead
#else
  static constexpr Dimension Dim1 = Dimension::Dim1;
  static constexpr Dimension Dim1Array = Dimension::Dim1Array;
  static constexpr Dimension Dim2 = Dimension::Dim2;
  static constexpr Dimension Dim2Array = Dimension::Dim2Array;
  static constexpr Dimension DimCube = Dimension::DimCube;
  static constexpr Dimension DimCubeArray = Dimension::DimCubeArray;
  static constexpr Dimension Dim3 = Dimension::Dim3;
#endif

  /// View descriptor.
  ///
  // TODO: Format/aspect
  struct Desc {
    Range levels;
    Range layers;
    Dimension dimension;
  };

  ImageView() = default;
  ImageView(const ImageView&) = delete;
  ImageView& operator=(const ImageView&) = delete;
  virtual ~ImageView() = default;

  /// Gets the image which this is a view of.
  ///
  virtual Image& image() = 0;

  /// Getters.
  ///
  virtual Range levels() const = 0;
  virtual Range layers() const = 0;
  virtual Dimension dimension() const = 0;
};

/// Formatted multidimensional data in device memory.
///
class Image {
 public:
  using Ptr = std::unique_ptr<Image>;

  /// Image dimensionality.
  ///
  enum class Dimension {
    Dim1,
    Dim2,
    Dim3
  };
  // TODO: Update this when migrating to C++20
#if __cplusplus >= 202002L
# error Use `using` instead
#else
  static constexpr Dimension Dim1 = Dimension::Dim1;
  static constexpr Dimension Dim2 = Dimension::Dim2;
  static constexpr Dimension Dim3 = Dimension::Dim3;
#endif

  /// Mask of `Usage` bits.
  ///
  using UsageMask = uint32_t;

  /// Usages of an image.
  ///
  enum Usage : uint32_t {
    CopySrc    = 0x01,
    CopyDst    = 0x02,
    Sampled    = 0x04,
    Storage    = 0x08,
    Attachment = 0x10
  };

  /// Image descriptor.
  ///
  struct Desc {
    Format format;
    Size3 size;
    uint32_t levels;
    Samples samples;
    Dimension dimension;
    UsageMask usageMask;
  };

  Image() = default;
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
  virtual ~Image() = default;

  /// Creates a new image view object.
  ///
  virtual ImageView::Ptr view(const ImageView::Desc& desc) = 0;

  /// Writes data to image memory.
  ///
  // TODO: Update for use with 3D images
  virtual void write(Offset2 offset, Size2 size, uint32_t layer, uint32_t level,
                     const void* data) = 0;

  /// Getters.
  ///
  virtual Format format() const = 0;
  virtual Size3 size() const = 0;
  virtual uint32_t levels() const = 0;
  virtual Samples samples() const = 0;
  virtual Dimension dimension() const = 0;
  virtual UsageMask usageMask() const = 0;

  /// Gets the number of bytes in a single texel of the `Format`.
  ///
  static uint32_t texelSize(Format format);
  uint32_t texelSize() const;
};

/// Wrap modes.
///
enum WrapMode {
  WrapModeClamp,
  WrapModeMirror,
  WrapModeRepeat
};

/// Filters.
///
enum Filter {
  FilterNearest,
  FilterLinear,
  FilterNearestNearest,
  FilterLinearNearest,
  FilterNearestLinear,
  FilterLinearLinear
};

/// Image sampler.
///
struct Sampler {
  WrapMode wrapU = WrapModeRepeat;
  WrapMode wrapV = WrapModeRepeat;
  WrapMode wrapW = WrapModeRepeat;
  Filter magFilter = FilterNearest;
  Filter minFilter = FilterNearest;

  bool operator==(const Sampler& other) const {
    return wrapU == other.wrapU && wrapV == other.wrapV &&
           wrapW == other.wrapW && magFilter == other.magFilter &&
           minFilter == other.minFilter;
  }

  bool operator!=(const Sampler& other) const {
    return !operator==(other);
  }
};

CG_NS_END

#endif // YF_CG_IMAGE_H
