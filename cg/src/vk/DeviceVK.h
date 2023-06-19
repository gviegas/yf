//
// CG
// DeviceVK.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#ifndef YF_CG_DEVICEVK_H
#define YF_CG_DEVICEVK_H

#include "Defs.h"
#include "Device.h"
#include "VK.h"

CG_NS_BEGIN

class QueueVK;
class DeviceVK;

/// Gets the device instance.
///
DeviceVK& deviceVK();

class DeviceVK final : public Device {
 public:
  ~DeviceVK();

  Queue& defaultQueue();
  Queue& queue(Queue::CapabilityMask capabilities);

  Buffer::Ptr buffer(uint64_t size, Buffer::Mode mode,
                     Buffer::UsageMask usageMask);

  Image::Ptr image(PxFormat format, Size2 size, uint32_t layers,
                   uint32_t levels, Samples samples);

  Shader::Ptr shader(Stage stage, const std::string& codeFile,
                     const std::string& entryPoint);

  DcTable::Ptr dcTable(const std::vector<DcEntry>& entries);

  Pass::Ptr pass(const std::vector<AttachDesc>* colors,
                 const std::vector<AttachDesc>* resolves,
                 const AttachDesc* depthStencil);

  GrState::Ptr state(const GrState::Config& config);
  CpState::Ptr state(const CpState::Config& config);

  Wsi::Ptr wsi(WS_NS::Window& window);

  const Limits& limits() const;

  /// Getters.
  ///
  VkInstance instance();
  VkPhysicalDevice physicalDev();
  VkDevice device();
  VkPipelineCache cache();
  const VkPhysicalDeviceProperties& physProperties() const;
  const VkPhysicalDeviceMemoryProperties& memProperties() const;
  const std::vector<const char*>& instExtensions() const;
  const std::vector<const char*>& devExtensions() const;
  const std::vector<const char*>& layers() const;
  uint32_t instVersion() const;
  uint32_t devVersion() const;
  const VkPhysicalDeviceFeatures& features() const;
  const VkPhysicalDeviceLimits& physLimits() const;

 private:
  QueueVK* queue_ = nullptr;

  VkInstance instance_ = nullptr;
  uint32_t instVersion_ = 0;
  std::vector<const char*> instExtensions_{};
  std::vector<const char*> layers_{};

  VkPhysicalDevice physicalDev_ = nullptr;
  VkPhysicalDeviceProperties physProperties_{};
  VkPhysicalDeviceMemoryProperties memProperties_{};
  VkPhysicalDeviceFeatures features_{};

  Limits limits_{};

  VkDevice device_ = nullptr;
  std::vector<const char*> devExtensions_{};

  VkPipelineCache cache_ = VK_NULL_HANDLE;

  friend DeviceVK& deviceVK();
  DeviceVK();

  bool checkInstanceExtensions();
  bool checkDeviceExtensions();
  void initInstance();
  void initPhysicalDevice();
  void initDevice(int32_t, int32_t);
  void setFeatures();
  void setLimits();
};

CG_NS_END

#endif // YF_CG_DEVICEVK_H
