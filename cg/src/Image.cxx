//
// CG
// Image.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Image.h"

using namespace CG_NS;

INTERNAL_NS_BEGIN

/// Computes the number of bytes in a pixel format's texel block.
///
inline uint32_t texelSize(PxFormat format) {
  switch (format) {
  case PxFormatR8Unorm:
  case PxFormatR8Uint:
  case PxFormatR8Srgb:
  case PxFormatS8Uint:
    return 1;

  case PxFormatRg8Unorm:
  case PxFormatRg8Uint:
  case PxFormatRg8Srgb:
  case PxFormatR16Unorm:
  case PxFormatR16Uint:
  case PxFormatD16Unorm:
    return 2;

  case PxFormatRgb8Unorm:
  case PxFormatRgb8Uint:
  case PxFormatRgb8Srgb:
  case PxFormatBgr8Unorm:
  case PxFormatBgr8Uint:
  case PxFormatBgr8Srgb:
  case PxFormatD16UnormS8Uint:
    return 3;

  case PxFormatRgba8Unorm:
  case PxFormatRgba8Uint:
  case PxFormatRgba8Srgb:
  case PxFormatBgra8Unorm:
  case PxFormatBgra8Uint:
  case PxFormatBgra8Srgb:
  case PxFormatRg16Unorm:
  case PxFormatRg16Uint:
  case PxFormatR32Uint:
  case PxFormatR32Flt:
  case PxFormatD32Flt:
  case PxFormatD24UnormS8Uint:
    return 4;

  default:
    return 0;
  }
}

INTERNAL_NS_END

Image::Image(PxFormat format,
             Size2 size,
             uint32_t layers,
             uint32_t levels,
             Samples samples)
  : format_(format), size_(size), layers_(layers), levels_(levels),
    samples_(samples), bitsPerTexel_(texelSize(format) << 3) { }

Image::~Image() { }
