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
class ImgView {
 public:
  using Ptr = std::unique_ptr<ImgView>;

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

  ImgView(Image& image, const Desc& desc);
  ImgView(const ImgView&) = delete;
  ImgView& operator=(const ImgView&) = delete;
  virtual ~ImgView() = 0;

  /// Gets the image which this is a view of.
  ///
  // TODO: Should be reference-counted.
  Image& image();

  /// Gets the level range.
  ///
  Range levels() const;

  /// Gets the layer range.
  ///
  Range layers() const;

  /// Gets the view dimension.
  ///
  Dimension dimension() const;

 private:
  Image& image_;
  const Range levels_;
  const Range layers_;
  const Dimension dimension_;
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

  Image(const Desc& desc);
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
  virtual ~Image() = default;

  /// Creates a new image view object.
  ///
  virtual ImgView::Ptr view(const ImgView::Desc& desc) = 0;

  /// Writes data to image memory.
  ///
  virtual void write(uint32_t plane, Origin3 origin, uint32_t level,
                     const void* data, Size3 size, uint32_t bytesPerRow = 0,
                     uint32_t rowsPerSlice = 0) = 0;

  /// Gets the pixel format.
  ///
  Format format() const;

  /// Gets the size of the image.
  ///
  Size3 size() const;

  /// Gets the number of mip levels in the image.
  ///
  uint32_t levels() const;

  /// Gets the sample count.
  ///
  Samples samples() const;

  /// Gets the image dimension.
  ///
  Dimension dimension() const;

  /// Gets the allowed usages for the image.
  ///
  UsageMask usageMask() const;

  /// Gets the number of bytes in a single texel of the `Format`.
  ///
  static uint32_t texelSize(Format format);
  uint32_t texelSize() const;

 private:
  const Format format_;
  const Size3 size_;
  const uint32_t levels_;
  const Samples samples_;
  const Dimension dimension_;
  const UsageMask usageMask_;
};

CG_NS_END

#endif // YF_CG_IMAGE_H
