//
// yf
// CGDevice.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_DEVICE_H
#define YF_CG_DEVICE_H

#include <memory>
#include <cstdint>

#include "YFDefs.h"
#include "CGBuffer.h"
#include "CGImage.h"
#include "CGShader.h"
#include "CGDcTable.h"
#include "CGPass.h"
#include "CGState.h"
#include "CGQueue.h"

YF_NS_BEGIN

class CGDevice {
 public:
  virtual ~CGDevice();

  /// Retrieves the device instance.
  ///
  static CGDevice& get();

  /// Makes a new buffer object.
  ///
  using BufferRes = CGResultPair<std::unique_ptr<CGBuffer>>;
  virtual BufferRes makeBuffer(uint64_t size) = 0;

  /// Makes a new image object.
  ///
  using ImageRes = CGResultPair<std::unique_ptr<CGImage>>;
  virtual ImageRes makeImage(CGPxFormat format,
                             CGSize2 size,
                             uint32_t layers,
                             uint32_t levels,
                             CGSamples samples) = 0;

  /// Makes a new shader object.
  ///
  using ShaderRes = CGResultPair<std::unique_ptr<CGShader>>;
  virtual ShaderRes makeShader(CGStage stage,
                               std::wstring&& codeFile,
                               std::wstring&& entryPoint = L"main") = 0;

  /// Makes a new descriptor table object.
  ///
  using DcTableRes = CGResultPair<std::unique_ptr<CGDcTable>>;
  virtual DcTableRes makeDcTable(const CGDcEntries& entries) = 0;
  virtual DcTableRes makeDcTable(CGDcEntries&& entries) = 0;

  /// Makes a new pass object.
  ///
  using PassRes = CGResultPair<std::unique_ptr<CGPass>>;
  virtual PassRes makePass(const std::vector<CGColorAttach>* colors,
                           const std::vector<CGColorAttach>* resolves,
                           const CGDepStenAttach* depthStencil) = 0;

  /// Makes a new state object.
  ///
  using GrStateRes = CGResultPair<std::unique_ptr<CGGrState>>;
  using CpStateRes = CGResultPair<std::unique_ptr<CGCpState>>;
  virtual GrStateRes makeState(const CGGrState::Config& config) = 0;
  virtual GrStateRes makeState(CGGrState::Config&& config) = 0;
  virtual CpStateRes makeState(const CGCpState::Config& config) = 0;
  virtual CpStateRes makeState(CGCpState::Config&& config) = 0;

  /// Retrieves execution queues.
  ///
  using QueueRes = CGResultPair<CGQueue*>;
  virtual QueueRes defaultQueue() = 0;
  virtual QueueRes queue(CGQueue::CapabilityMask capabilities) = 0;
};

YF_NS_END

#endif // YF_CG_DEVICE_H
