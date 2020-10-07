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
#include "CGShader.h"

YF_NS_BEGIN

class CGDevice {
 public:
  virtual ~CGDevice();

  /// Retrieves the device instance.
  ///
  static CGDevice& get();

  /// Makes a new buffer object.
  ///
  using BufferPtr = std::unique_ptr<CGBuffer>;
  virtual BufferPtr makeBuffer(uint64_t size) = 0;

  /// Makes a new image object.
  ///
  using ImagePtr = std::unique_ptr<CGImage>;
  virtual ImagePtr makeImage(CGPxFormat format,
                             CGSize2 size,
                             uint32_t layers,
                             uint32_t levels,
                             CGSamples samples) = 0;

  /// Makes a new shader object.
  ///
  using ShaderPtr = std::unique_ptr<CGShader>;
  virtual ShaderPtr makeShader(CGStage stage,
                               std::wstring&& codeFile,
                               std::wstring&& entryPoint = L"main") = 0;
};

YF_NS_END

#endif // YF_CG_DEVICE_H
