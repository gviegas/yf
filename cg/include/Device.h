//
// CG
// Device.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_DEVICE_H
#define YF_CG_DEVICE_H

#include <cstdint>
#include <memory>

#include "yf/cg/Defs.h"
#include "yf/cg/Queue.h"
#include "yf/cg/Buffer.h"
#include "yf/cg/Image.h"
#include "yf/cg/Shader.h"
#include "yf/cg/DcTable.h"
#include "yf/cg/Pass.h"
#include "yf/cg/State.h"
#include "yf/cg/Wsi.h"
#include "yf/cg/Limits.h"

CG_NS_BEGIN

/// Device abstraction.
///
class Device {
 public:
  virtual ~Device();

  /// Gets execution queues.
  ///
  virtual Queue& defaultQueue() = 0;
  virtual Queue& queue(Queue::CapabilityMask capabilities) = 0;

  /// Creates a new buffer object.
  ///
  virtual Buffer::Ptr buffer(uint64_t size) = 0;

  /// Creates a new image object.
  ///
  virtual Image::Ptr image(PxFormat format, Size2 size, uint32_t layers,
                           uint32_t levels, Samples samples) = 0;

  /// Creates a new shader object.
  ///
  virtual Shader::Ptr shader(Stage stage, const std::string& codeFile,
                             const std::string& entryPoint = "main") = 0;

  /// Creates a new descriptor table object.
  ///
  virtual DcTable::Ptr dcTable(const std::vector<DcEntry>& entries) = 0;

  /// Creates a new pass object.
  ///
  virtual Pass::Ptr pass(const std::vector<ColorAttach>* colors,
                         const std::vector<ColorAttach>* resolves,
                         const DepStenAttach* depthStencil) = 0;

  /// Creates a new state object.
  ///
  virtual GrState::Ptr state(const GrState::Config& config) = 0;
  virtual CpState::Ptr state(const CpState::Config& config) = 0;

  /// Creates a new wsi object.
  ///
  virtual Wsi::Ptr wsi(WS_NS::Window* window) = 0;

  /// Gets limits.
  ///
  virtual const Limits& limits() const = 0;
};

/// Gets the device instance.
///
Device& device();

CG_NS_END

#endif // YF_CG_DEVICE_H
