//
// CG
// Image.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
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
enum PxFormat {
  PxFormatUndefined,

  PxFormatR8Unorm,
  PxFormatR8Uint,
  PxFormatR8Srgb,

  PxFormatRg8Unorm,
  PxFormatRg8Uint,
  PxFormatRg8Srgb,

  PxFormatRgb8Unorm,
  PxFormatRgb8Uint,
  PxFormatRgb8Srgb,
  PxFormatBgr8Unorm,
  PxFormatBgr8Uint,
  PxFormatBgr8Srgb,

  PxFormatRgba8Unorm,
  PxFormatRgba8Uint,
  PxFormatRgba8Srgb,
  PxFormatBgra8Unorm,
  PxFormatBgra8Uint,
  PxFormatBgra8Srgb,

  PxFormatR16Unorm,
  PxFormatR16Uint,

  PxFormatRg16Unorm,
  PxFormatRg16Uint,

  PxFormatRgb16Unorm,
  PxFormatRgb16Uint,

  PxFormatRgba16Unorm,
  PxFormatRgba16Uint,

  PxFormatR32Uint,
  PxFormatR32Flt,

  PxFormatD16Unorm,
  PxFormatD32Flt,

  PxFormatS8Uint,

  PxFormatD16UnormS8Uint,
  PxFormatD24UnormS8Uint
};

/// Sample count.
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

/// Formatted multidimensional data in device memory.
///
class Image {
 public:
  using Ptr = std::unique_ptr<Image>;

  Image(PxFormat format, Size2 size, uint32_t layers, uint32_t levels,
        Samples samples);

  virtual ~Image();

  /// Writes data to image.
  ///
  virtual void write(Offset2 offset, Size2 size, uint32_t layer, uint32_t level,
                     const void* data) = 0;

  /// Instance constants.
  ///
  const PxFormat format_;
  const Size2 size_;
  const uint32_t layers_;
  const uint32_t levels_;
  const Samples samples_;

  /// The number of bits per texel in the image format.
  ///
  const uint32_t bitsPerTexel_;
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
