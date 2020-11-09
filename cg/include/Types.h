//
// CG
// Types.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_TYPES_H
#define YF_CG_TYPES_H

#include <cstdint>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

/// Two-dimensional size.
///
struct Size2 {
  Size2(uint32_t width, uint32_t height) : width(width), height(height) {}
  Size2(uint32_t size) : width(size), height(size) {}

  bool operator==(const Size2& other) const {
    return width == other.width && height == other.height;
  }
  bool operator!=(const Size2& other) const {
    return !operator==(other);
  }

  uint32_t width;
  uint32_t height;
};

/// Three-dimensional size.
///
struct Size3 : Size2 {
  Size3(uint32_t width, uint32_t height, uint32_t depth)
    : Size2(width, height), depth(depth) {}
  Size3(Size2 size2, uint32_t depth) : Size2(size2), depth(depth) {}
  Size3(uint32_t size) : Size2(size), depth(size) {}

  bool operator==(const Size3& other) const {
    return Size2::operator==(other) && depth == other.depth;
  }
  bool operator!=(const Size3& other) const {
    return !operator==(other);
  }

  uint32_t depth;
};

/// Two-dimensional offset.
///
struct Offset2 {
  Offset2(int32_t x, int32_t y) : x(x), y(y) {}
  Offset2(int32_t value = 0) : x(value), y(value) {}

  bool operator==(const Offset2& other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(const Offset2& other) const {
    return !operator==(other);
  }

  int32_t x;
  int32_t y;
};

/// Three-dimensional offset.
///
struct Offset3 : Offset2 {
  Offset3(int32_t x, int32_t y, int32_t z) : Offset2(x, y), z(z) {}
  Offset3(Offset2 offset2, int32_t z) : Offset2(offset2), z(z) {}
  Offset3(int32_t value = 0) : Offset3(value, value, value) {}

  bool operator==(const Offset3& other) const {
    return Offset2::operator==(other) && z == other.z;
  }
  bool operator!=(const Offset3& other) const {
    return !operator==(other);
  }

  int32_t z;
};

CG_NS_END

#endif // YF_CG_TYPES_H
