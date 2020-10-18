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

  CGBuffer::Ptr makeBuffer(uint64_t size);

  CGImage::Ptr makeImage(CGPxFormat format,
                         CGSize2 size,
                         uint32_t layers,
                         uint32_t levels,
                         CGSamples samples);

  CGShader::Ptr makeShader(CGStage stage,
                           std::wstring&& codeFile,
                           std::wstring&& entryPoint);

  CGDcTable::Ptr makeDcTable(const CGDcEntries& entries);
  CGDcTable::Ptr makeDcTable(CGDcEntries&& entries);

  CGPass::Ptr makePass(const std::vector<CGColorAttach>* colors,
                       const std::vector<CGColorAttach>* resolves,
                       const CGDepStenAttach* depthStencil);

  CGGrState::Ptr makeState(const CGGrState::Config& config);
  CGGrState::Ptr makeState(CGGrState::Config&& config);
  CGCpState::Ptr makeState(const CGCpState::Config& config);
  CGCpState::Ptr makeState(CGCpState::Config&& config);

  CGQueue& defaultQueue();
  CGQueue& queue(CGQueue::CapabilityMask capabilities);

  // TODO: getters

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
  VkQueue _graphQueue = nullptr;
  VkQueue _compQueue = nullptr;
};

YF_NS_END

#endif // YF_CG_DEVICEVK_H
