//
// CG
// Image.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#include <stdexcept>

#include "Image.h"

using namespace CG_NS;
using namespace std;

uint32_t Image::texelSize(PxFormat format) {
  switch (format) {
  case PxFormatR8Unorm:
  case PxFormatR8Norm:
  case PxFormatR8Uint:
  case PxFormatR8Int:
  case PxFormatS8:
    return 1;

  case PxFormatR16Uint:
  case PxFormatR16Int:
  case PxFormatR16Float:
  case PxFormatRg8Unorm:
  case PxFormatRg8Norm:
  case PxFormatRg8Uint:
  case PxFormatRg8Int:
  case PxFormatD16Unorm:
    return 2;

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
  case PxFormatD32Float:
  case PxFormatD24UnormS8:
    return 4;

  case PxFormatRg32Uint:
  case PxFormatRg32Int:
  case PxFormatRg32Float:
  case PxFormatRgba16Uint:
  case PxFormatRgba16Int:
  case PxFormatRgba16Float:
    return 8;

  case PxFormatRgba32Uint:
  case PxFormatRgba32Int:
  case PxFormatRgba32Float:
    return 16;

  case PxFormatD32FloatS8:
    return 5;

  case PxFormatUndefined:
  default:
    throw invalid_argument(__func__);
  }
}

uint32_t Image::texelSize() const {
  return texelSize(format());
}
