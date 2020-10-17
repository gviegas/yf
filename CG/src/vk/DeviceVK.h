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

/*
  VkInstance instance() const;
  VkPhysicalDevice physicalDev() const;
  VkDevice device() const;
  const VkPhysicalDeviceProperties& physProperties() const;
  uint32_t instVersion();
  uint32_t devVersion();
  const std::vector<const char*>& layers() const;
  const std::vector<const char*>& instExtensions() const;
  const std::vector<const char*>& devExtensions() const;
*/

 private:
  DeviceVK();

  CGResult checkInstanceExtensions();
  CGResult checkDeviceExtensions();
  void initInstance();
  void initPhysicalDevice();
  void initDevice();

  VkInstance _instance = nullptr;
  uint32_t _instVersion = 0;
  std::vector<const char*> _instExtensions{};
  std::vector<const char*> _layers{};

  VkPhysicalDevice _physicalDev = nullptr;
  VkPhysicalDeviceProperties _physProperties{};
  int32_t _graphFamily = -1;
  int32_t _compFamily = -1;

  VkDevice _device = nullptr;
  std::vector<const char*> _devExtensions{};
};

YF_NS_END

#endif // YF_CG_DEVICEVK_H
