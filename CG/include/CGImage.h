//
// yf
// CGImage.h
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_IMAGE_H
#define YF_CG_IMAGE_H

#include <cstdint>

#include "YFDefs.h"
#include "CGResult.h"

YF_NS_BEGIN

/// Pixel formats.
///
enum CGPxFormat {
  // TODO
  CGPxFormatUndefined,
  CGPxFormatBgra8Srgb,
  CGPxFormatRgba8Unorm,
  CGPxFormatD16S8Unorm
};

/// Sample count.
///
enum CGSamples {
  CGSamples1,
  CGSamples2,
  CGSamples4,
  CGSamples8,
  CGSamples16,
  CGSamples32,
  CGSamples64
};

/// Two-dimensional size.
///
struct CGSize2 {
  CGSize2(uint32_t w, uint32_t h) : width(w), height(h) {}
  CGSize2(uint32_t wh) : width(wh), height(wh) {} // implicit

  bool operator ==(const CGSize2& other) const {
    return width == other.width && height == other.height;
  }

  uint32_t width;
  uint32_t height;
};

/// Two-dimensional offset.
///
struct CGOffset2 {
  bool operator ==(const CGOffset2& other) const {
    return x == other.x && y == other.y;
  }

  int32_t x;
  int32_t y;
};

class CGImage {
 public:
  CGImage(CGPxFormat format,
          CGSize2 size,
          uint32_t layers,
          uint32_t levels,
          CGSamples samples);

  virtual ~CGImage();

  /// Writes data to image.
  ///
  virtual CGResult write(CGOffset2 offset,
                         CGSize2 size,
                         uint32_t layer,
                         uint32_t level,
                         const void* data) = 0;

  /// Instance constants.
  ///
  const CGPxFormat _format;
  const CGSize2     _size;
  const uint32_t    _layers;
  const uint32_t    _levels;
  const CGSamples   _samples;
};

YF_NS_END

#endif // YF_CG_IMAGE_H
