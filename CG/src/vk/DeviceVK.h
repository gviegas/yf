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
#include "VK.h"

YF_NS_BEGIN

class DeviceVK final : public CGDevice {
 public:
  ~DeviceVK();

  static DeviceVK& get();

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

  VkInstance instance() const;
  VkPhysicalDevice physicalDev() const;
  VkDevice device() const;
  uint32_t instVersion();
  uint32_t devVersion();
  const std::vector<const char*>& instLayers() const;
  const std::vector<const char*>& instExts() const;
  const std::vector<const char*>& devExts() const;

 private:
  DeviceVK();

  CGResult checkInstanceExts();
  CGResult checkDeviceExts();
  void initInstance();
  void initDevice();

  VkInstance _instance;
  VkPhysicalDevice _physicalDev;
  VkDevice _device;
  uint32_t _instVersion;
  uint32_t _devVersion;
  std::vector<const char*> _instLayers;
  std::vector<const char*> _instExts;
  std::vector<const char*> _devExts;
};

YF_NS_END

#endif // YF_CG_DEVICEVK_H
