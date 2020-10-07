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
#include "CGTypes.h"
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
  const CGPxFormat format;
  const CGSize2    size;
  const uint32_t   layers;
  const uint32_t   levels;
  const CGSamples  samples;
};

YF_NS_END

#endif // YF_CG_IMAGE_H
