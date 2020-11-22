//
// CG
// Device.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_DEVICE_H
#define YF_CG_DEVICE_H

#include <memory>
#include <cstdint>

#include "yf/cg/Defs.h"
#include "yf/cg/Queue.h"
#include "yf/cg/Buffer.h"
#include "yf/cg/Image.h"
#include "yf/cg/Shader.h"
#include "yf/cg/DcTable.h"
#include "yf/cg/Pass.h"
#include "yf/cg/State.h"
#include "yf/cg/Wsi.h"

CG_NS_BEGIN

/// Device abstraction.
///
class Device {
 public:
  virtual ~Device();

  /// Gets the device instance.
  ///
  static Device& get();

  /// Gets execution queues.
  ///
  virtual Queue& defaultQueue() = 0;
  virtual Queue& queue(Queue::CapabilityMask capabilities) = 0;

  /// Makes a new buffer object.
  ///
  virtual Buffer::Ptr makeBuffer(uint64_t size) = 0;

  /// Makes a new image object.
  ///
  virtual Image::Ptr makeImage(PxFormat format,
                               Size2 size,
                               uint32_t layers,
                               uint32_t levels,
                               Samples samples) = 0;

  /// Makes a new shader object.
  ///
  virtual Shader::Ptr makeShader(Stage stage,
                                 std::wstring&& codeFile,
                                 std::wstring&& entryPoint = L"main") = 0;

  /// Makes a new descriptor table object.
  ///
  virtual DcTable::Ptr makeDcTable(const DcEntries& entries) = 0;

  /// Makes a new pass object.
  ///
  virtual Pass::Ptr makePass(const std::vector<ColorAttach>* colors,
                             const std::vector<ColorAttach>* resolves,
                             const DepStenAttach* depthStencil) = 0;

  /// Makes a new state object.
  ///
  virtual GrState::Ptr makeState(const GrState::Config& config) = 0;
  virtual CpState::Ptr makeState(const CpState::Config& config) = 0;

  /// Makes a new wsi object.
  ///
  virtual Wsi::Ptr makeWsi(WS_NS::Window* window) = 0;
};

CG_NS_END

#endif // YF_CG_DEVICE_H
