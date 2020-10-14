//
// yf
// DeviceVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_DEVICEVK_H
#define YF_CG_DEVICEVK_H

#include "YFDefs.h"
#include "CGDevice.h"

YF_NS_BEGIN

class DeviceVK : public CGDevice {
 public:
  DeviceVK();
  ~DeviceVK();

  BufferRes makeBuffer(uint64_t size);

  ImageRes makeImage(CGPxFormat format,
                     CGSize2 size,
                     uint32_t layers,
                     uint32_t levels,
                     CGSamples samples);

  ShaderRes makeShader(CGStage stage,
                       std::wstring&& codeFile,
                       std::wstring&& entryPoint);

  DcTableRes makeDcTable(const CGDcEntries& entries);
  DcTableRes makeDcTable(CGDcEntries&& entries);

  PassRes makePass(const std::vector<CGColorAttach>* colors,
                   const std::vector<CGColorAttach>* resolves,
                   const CGDepStenAttach* depthStencil);

  GrStateRes makeState(const CGGrState::Config& config);
  GrStateRes makeState(CGGrState::Config&& config);
  CpStateRes makeState(const CGCpState::Config& config);
  CpStateRes makeState(CGCpState::Config&& config);

  QueueRes defaultQueue();
  QueueRes queue(CGQueue::CapabilityMask capabilities);

 private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};

YF_NS_END

#endif // YF_CG_DEVICEVK_H
