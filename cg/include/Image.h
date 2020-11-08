//
// cg
// Image.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_IMAGE_H
#define YF_CG_IMAGE_H

#include <cstdint>
#include <memory>

#include "yf/cg/Defs.h"
#include "yf/cg/Types.h"
#include "yf/cg/Result.h"

CG_NS_BEGIN

/// Pixel formats.
///
enum PxFormat {
  // TODO
  PxFormatUndefined,
  PxFormatBgra8Srgb,
  PxFormatRgba8Unorm,
  PxFormatD16Unorm
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

  Image(PxFormat format,
        Size2 size,
        uint32_t layers,
        uint32_t levels,
        Samples samples);

  virtual ~Image();

  /// Writes data to image.
  ///
  virtual Result write(Offset2 offset,
                       Size2 size,
                       uint32_t layer,
                       uint32_t level,
                       const void* data) = 0;

  /// Instance constants.
  ///
  const PxFormat format;
  const Size2    size;
  const uint32_t layers;
  const uint32_t levels;
  const Samples  samples;
};

CG_NS_END

#endif // YF_CG_IMAGE_H
