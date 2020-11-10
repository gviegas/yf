//
// CG
// DeviceVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_DEVICEVK_H
#define YF_CG_DEVICEVK_H

#include "yf/cg/Defs.h"
#include "yf/cg/Device.h"
#include "VK.h"

CG_NS_BEGIN

class QueueVK;

class DeviceVK final : public Device {
 public:
  ~DeviceVK();

  static DeviceVK& get();

  Buffer::Ptr makeBuffer(uint64_t size);

  Image::Ptr makeImage(PxFormat format,
                       Size2 size,
                       uint32_t layers,
                       uint32_t levels,
                       Samples samples);

  Shader::Ptr makeShader(Stage stage,
                         std::wstring&& codeFile,
                         std::wstring&& entryPoint);

  DcTable::Ptr makeDcTable(const DcEntries& entries);

  Pass::Ptr makePass(const std::vector<ColorAttach>* colors,
                     const std::vector<ColorAttach>* resolves,
                     const DepStenAttach* depthStencil);

  GrState::Ptr makeState(const GrState::Config& config);
  CpState::Ptr makeState(const CpState::Config& config);

  Queue& defaultQueue();
  Queue& queue(Queue::CapabilityMask capabilities);

  /// Getters.
  ///
  VkInstance instance() const;
  VkPhysicalDevice physicalDev() const;
  VkDevice device() const;
  const VkPhysicalDeviceProperties& physProperties() const;
  const VkPhysicalDeviceMemoryProperties& memProperties() const;
  const std::vector<const char*>& instExtensions() const;
  const std::vector<const char*>& devExtensions() const;
  const std::vector<const char*>& layers() const;
  uint32_t instVersion() const;
  uint32_t devVersion() const;
  const VkPhysicalDeviceLimits& limits() const;

 private:
  DeviceVK();

  bool checkInstanceExtensions();
  bool checkDeviceExtensions();
  void initInstance();
  void initPhysicalDevice();
  void initDevice(int32_t);

  VkInstance instance_ = nullptr;
  uint32_t instVersion_ = 0;
  std::vector<const char*> instExtensions_{};
  std::vector<const char*> layers_{};

  VkPhysicalDevice physicalDev_ = nullptr;
  VkPhysicalDeviceProperties physProperties_{};
  VkPhysicalDeviceMemoryProperties memProperties_{};

  VkDevice device_ = nullptr;
  std::vector<const char*> devExtensions_{};

  QueueVK* queue_ = nullptr;
};

CG_NS_END

#endif // YF_CG_DEVICEVK_H
