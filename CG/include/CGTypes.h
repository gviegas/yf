//
// yf
// CGTypes.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_TYPES_H
#define YF_CG_TYPES_H

#include <cstdint>

#include "YFDefs.h"

YF_NS_BEGIN

/// Two-dimensional size.
///
struct CGSize2 {
  CGSize2(uint32_t width, uint32_t height) : width(width), height(height) {}
  CGSize2(uint32_t size) : width(size), height(size) {}

  bool operator==(const CGSize2& other) const {
    return width == other.width && height == other.height;
  }
  bool operator!=(const CGSize2& other) const {
    return !operator==(other);
  }

  uint32_t width;
  uint32_t height;
};

/// Three-dimensional size.
///
struct CGSize3 : CGSize2 {
  CGSize3(uint32_t width, uint32_t height, uint32_t depth)
    : CGSize2(width, height), depth(depth) {}
  CGSize3(CGSize2 size2, uint32_t depth) : CGSize2(size2), depth(depth) {}
  CGSize3(uint32_t size) : CGSize2(size), depth(size) {}

  bool operator==(const CGSize3& other) const {
    return CGSize2::operator==(other) && depth == other.depth;
  }
  bool operator!=(const CGSize3& other) const {
    return !operator==(other);
  }

  uint32_t depth;
};

/// Two-dimensional offset.
///
struct CGOffset2 {
  CGOffset2(int32_t x, int32_t y) : x(x), y(y) {}
  CGOffset2(int32_t value = 0) : x(value), y(value) {}

  bool operator==(const CGOffset2& other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(const CGOffset2& other) const {
    return !operator==(other);
  }

  int32_t x;
  int32_t y;
};

/// Three-dimensional offset.
///
struct CGOffset3 : CGOffset2 {
  CGOffset3(int32_t x, int32_t y, int32_t z) : CGOffset2(x, y), z(z) {}
  CGOffset3(CGOffset2 offset2, int32_t z) : CGOffset2(offset2), z(z) {}
  CGOffset3(int32_t value = 0) : CGOffset3(value, value, value) {}

  bool operator==(const CGOffset3& other) const {
    return CGOffset2::operator==(other) && z == other.z;
  }
  bool operator!=(const CGOffset3& other) const {
    return !operator==(other);
  }

  int32_t z;
};

YF_NS_END

#endif // YF_CG_TYPES_H
