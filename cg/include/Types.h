//
// CG
// Types.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#ifndef YF_CG_TYPES_H
#define YF_CG_TYPES_H

#include <cstdint>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

/// Two-dimensional size.
///
struct Size2 {
  uint32_t width;
  uint32_t height;

  Size2(uint32_t width, uint32_t height) : width(width), height(height) { }

  bool operator==(const Size2& other) const {
    return width == other.width && height == other.height;
  }

  bool operator!=(const Size2& other) const {
    return !operator==(other);
  }
};

/// Three-dimensional size.
///
struct Size3 : Size2 {
  uint32_t depthOrLayers;

  Size3(uint32_t width, uint32_t height, uint32_t depthOrLayers)
    : Size2(width, height), depthOrLayers(depthOrLayers) { }

  Size3(Size2 size2, uint32_t depthOrLayers)
    : Size2(size2), depthOrLayers(depthOrLayers) { }

  bool operator==(const Size3& other) const {
    return Size2::operator==(other) && depthOrLayers == other.depthOrLayers;
  }

  bool operator!=(const Size3& other) const {
    return !operator==(other);
  }
};

/// Two-dimensional offset.
///
struct Offset2 {
  int32_t x;
  int32_t y;

  Offset2(int32_t x, int32_t y) : x(x), y(y) { }
  Offset2() : Offset2(0, 0) { }

  bool operator==(const Offset2& other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Offset2& other) const {
    return !operator==(other);
  }
};

/// Three-dimensional offset.
///
struct Offset3 : Offset2 {
  int32_t z;

  Offset3(int32_t x, int32_t y, int32_t z) : Offset2(x, y), z(z) { }
  Offset3(Offset2 offset2, int32_t z) : Offset2(offset2), z(z) { }
  Offset3() : Offset3(0, 0, 0) { }

  bool operator==(const Offset3& other) const {
    return Offset2::operator==(other) && z == other.z;
  }

  bool operator!=(const Offset3& other) const {
    return !operator==(other);
  }
};

CG_NS_END

#endif // YF_CG_TYPES_H
