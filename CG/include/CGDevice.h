//
// yf
// CGDevice.h
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_DEVICE_H
#define YF_CG_DEVICE_H

#include <memory>
#include <cstdint>

#include "YFDefs.h"
#include "CGBuffer.h"
#include "CGImage.h"

YF_NS_BEGIN

class CGDevice {
 public:
  virtual ~CGDevice();

  /// Retrieves the device instance.
  ///
  static CGDevice& get();

  /// Makes a new buffer object.
  ///
  virtual std::unique_ptr<CGBuffer> makeBuffer(uint64_t size) = 0;

  /// Makes a new image object.
  ///
  virtual std::unique_ptr<CGImage> makeImage(CGPxFormat format,
                                             CGSize2 size,
                                             uint32_t layers,
                                             uint32_t levels,
                                             CGSamples samples) = 0;
};

YF_NS_END

#endif // YF_CG_DEVICE_H
