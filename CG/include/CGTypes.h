//
// yf
// CGTypes.h
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_TYPES_H
#define YF_CG_TYPES_H

#include <cstdint>

#include "YFDefs.h"

YF_NS_BEGIN

/// Two-dimensional size.
///
struct CGSize2 {
  CGSize2(uint32_t w, uint32_t h) : width(w), height(h) {}
  CGSize2(uint32_t s) : width(s), height(s) {} // implicit

  bool operator==(const CGSize2& other) const {
    return width == other.width && height == other.height;
  }

  uint32_t width;
  uint32_t height;
};

/// Three-dimensional size.
///
struct CGSize3 : CGSize2 {
  CGSize3(uint32_t w, uint32_t h, uint32_t d) : CGSize2(w, h), depth(d) {}
  CGSize3(CGSize2 size2, uint32_t d) : CGSize2(size2), depth(d) {}
  CGSize3(uint32_t s) : CGSize2(s), depth(s) {} // implicit

  bool operator==(const CGSize3 other) const {
    return CGSize2::operator==(other) && depth == other.depth;
  }

  uint32_t depth;
};

/// Two-dimensional offset.
///
struct CGOffset2 {
  CGOffset2(int32_t x, int32_t y) : x(x), y(y) {}
  CGOffset2(int32_t v = 0) : x(v), y(v) {} // implicit

  bool operator==(const CGOffset2& other) const {
    return x == other.x && y == other.y;
  }

  int32_t x;
  int32_t y;
};

/// Three-dimensional offset.
///
struct CGOffset3 : CGOffset2 {
  CGOffset3(int32_t x, int32_t y, int32_t z) : CGOffset2(x, y), z(z) {}
  CGOffset3(CGOffset2 off2, int32_t z) : CGOffset2(off2), z(z) {}
  CGOffset3(int32_t v = 0) : CGOffset3(v, v, v) {} // implicit

  bool operator==(const CGOffset3& other) const {
    return CGOffset2::operator==(other) && z == other.z;
  }

  int32_t z;
};

YF_NS_END

#endif // YF_CG_TYPES_H
