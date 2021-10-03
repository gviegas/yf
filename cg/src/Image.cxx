//
// CG
// Image.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <stdexcept>

#include "Image.h"

using namespace CG_NS;
using namespace std;

Image::~Image() { }

uint32_t Image::texelSize(PxFormat format) {
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

  case PxFormatRgb16Unorm:
  case PxFormatRgb16Uint:
    return 6;

  case PxFormatRgba16Unorm:
  case PxFormatRgba16Uint:
    return 8;

  default:
    throw invalid_argument(__func__);
  }
}

uint32_t Image::texelSize() const {
  return texelSize(format());
}
