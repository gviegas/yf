//
// CG
// DeviceVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cassert>
#include <unordered_set>
#include <string>
#include <stdexcept>

#include "DeviceVK.h"
#include "VK.h"
#include "QueueVK.h"
#include "BufferVK.h"
#include "ShaderVK.h"
#include "PassVK.h"

using namespace CG_NS;
using namespace std;

DeviceVK& DeviceVK::get() {
  static DeviceVK dev;
  return dev;
}

DeviceVK::DeviceVK() {
  if (!initVK())
    // TODO
    throw runtime_error("Failed to initialize VK lib");
  initInstance();
}

DeviceVK::~DeviceVK() {
  if (device_ != nullptr) {
    vkDeviceWaitIdle(device_);
    // TODO: ensure that all VK objects were disposed of prior to this point
    delete queue_;
    vkDestroyDevice(device_, nullptr);
  }
  if (instance_ != nullptr)
    vkDestroyInstance(instance_, nullptr);
}

bool DeviceVK::checkInstanceExtensions() {
  assert(instExtensions_.empty());

  setProcsVK(static_cast<VkInstance>(nullptr));

  // Set required extensions
  instExtensions_.push_back(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME);
  instExtensions_.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(__linux__)
  instExtensions_.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
  instExtensions_.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(__APPLE__)
  instExtensions_.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
  instExtensions_.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
# error "Invalid platform"
#endif

  // Get available extensions
  vector<VkExtensionProperties> exts;
  uint32_t extN;
  VkResult res;
  res = vkEnumerateInstanceExtensionProperties(nullptr, &extN, nullptr);
  if (res == VK_SUCCESS && extN > 0) {
    exts.resize(extN);
    res = vkEnumerateInstanceExtensionProperties(nullptr, &extN, exts.data());
    if (res != VK_SUCCESS)
      extN = 0;
  }

  // Check if the instance has everything
  unordered_set<string> reqExts;
  for (const auto& e : instExtensions_)
    reqExts.insert(e);
  for (const auto& e : exts)
    reqExts.erase(e.extensionName);

  return reqExts.empty() ? true : false;
}

bool DeviceVK::checkDeviceExtensions() {
  assert(physicalDev_ != nullptr);
  assert(devExtensions_.empty());

  // Set required extensions
  devExtensions_.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  // Get available extensions
  vector<VkExtensionProperties> exts;
  uint32_t extN;
  VkResult res;
  res = vkEnumerateDeviceExtensionProperties(physicalDev_, nullptr, &extN,
                                             nullptr);
  if (res == VK_SUCCESS && extN > 0) {
    exts.resize(extN);
    res = vkEnumerateDeviceExtensionProperties(physicalDev_, nullptr, &extN,
                                               exts.data());
    if (res != VK_SUCCESS)
      extN = 0;
  }

  // Check if the device has everything
  unordered_set<string> reqExts;
  for (const auto& e : devExtensions_)
    reqExts.insert(e);
  for (const auto& e : exts)
    reqExts.erase(e.extensionName);

  return reqExts.empty() ? true : false;
}

void DeviceVK::initInstance() {
  assert(instance_ == nullptr);

  // Check extensions
  if (!checkInstanceExtensions())
    // TODO
    throw runtime_error("Missing required instance extension(s)");

  // Get instance version
  if (!vkEnumerateInstanceVersion)
    instVersion_ = VK_API_VERSION_1_0;
  else
    vkEnumerateInstanceVersion(&instVersion_);

  // Create instance
  VkApplicationInfo appInfo;
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pNext = nullptr;
  appInfo.pApplicationName = "yf";
  appInfo.applicationVersion = 0;
  appInfo.pEngineName = nullptr;
  appInfo.engineVersion = 0;
  appInfo.apiVersion = instVersion_;

  VkInstanceCreateInfo instInfo;
  instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instInfo.pNext = nullptr;
  instInfo.flags = 0;
  instInfo.pApplicationInfo = &appInfo;
  instInfo.enabledLayerCount = layers_.size();
  instInfo.ppEnabledLayerNames = layers_.data();
  instInfo.enabledExtensionCount = instExtensions_.size();
  instInfo.ppEnabledExtensionNames = instExtensions_.data();

  auto res = vkCreateInstance(&instInfo, nullptr, &instance_);
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Failed to create VK instance");

  // Now the physical device can be initialized
  setProcsVK(instance_);
  initPhysicalDevice();
}

void DeviceVK::initPhysicalDevice() {
  assert(instance_ != nullptr);
  assert(physicalDev_ == nullptr);

  VkResult res;

  // Enumerate physical devices & get their properties
  vector<VkPhysicalDevice> phys;
  uint32_t physN;
  res = vkEnumeratePhysicalDevices(instance_, &physN, nullptr);
  if (res != VK_SUCCESS || physN == 0)
    // TODO
    throw runtime_error("Could not enumerate physical devices");
  phys.resize(physN);
  res = vkEnumeratePhysicalDevices(instance_, &physN, phys.data());
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Could not enumerate physical devices");

  vector<pair<uint32_t, VkPhysicalDeviceProperties>> physProps;
  physProps.resize(physN);
  for (uint32_t i = 0; i < physN; ++i) {
    physProps[i].first = i;
    vkGetPhysicalDeviceProperties(phys[i], &physProps[i].second);
  }

  // FIXME: too many assumptions
  sort(physProps.begin(), physProps.end(), [](auto& p1, auto& p2) {
    const auto t1 = p1.second.deviceType;
    const auto t2 = p2.second.deviceType;
    const auto v1 = p1.second.apiVersion;
    const auto v2 = p2.second.apiVersion;
    decltype(t1) types[] = {
      VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
      VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
      VK_PHYSICAL_DEVICE_TYPE_CPU
    };
    for (auto& t : types) {
      if (t1 == t)
        return t1 != t2 || v1 >= v2;
      if (t2 == t)
        return false;
    }
    return v1 >= v2;
  });

  // Find a physical device that supports both graphics and compute
  int32_t queueFamily = -1;
  for (const auto& p : physProps) {
    vector<VkQueueFamilyProperties> families;
    uint32_t familyN;
    vkGetPhysicalDeviceQueueFamilyProperties(phys[p.first], &familyN, nullptr);
    families.resize(familyN);
    vkGetPhysicalDeviceQueueFamilyProperties(phys[p.first], &familyN,
                                             families.data());

    for (uint32_t i = 0; i < familyN; ++i) {
      if (families[i].queueFlags &
      (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) {
        queueFamily = i;
        break;
      }
    }

    // [1.2.146 c4.1]
    // "If an implementation exposes any queue family that supports graphics
    // operations, at least one queue family of at least one physical device
    // exposed by the implementation must support both graphics and compute
    // operations."

    if (queueFamily > -1) {
      physicalDev_ = phys[p.first];
      physProperties_ = p.second;
      break;
    }
  }

  if (physicalDev_ == nullptr)
    // TODO
    throw runtime_error("Could not find a suitable physical device");

  vkGetPhysicalDeviceMemoryProperties(physicalDev_, &memProperties_);

  // Now the logical device can be initialized
  initDevice(queueFamily);
}

void DeviceVK::initDevice(int32_t queueFamily) {
  assert(queueFamily > -1);
  assert(physicalDev_ != nullptr);
  assert(device_ == nullptr);

  // Check extensions
  if (!checkDeviceExtensions())
    // TODO
    throw runtime_error("Missing required device extension(s)");

  // Define queue
  const float queuePrio = 0.0f;
  VkDeviceQueueCreateInfo queueInfo;
  queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueInfo.pNext = nullptr;
  queueInfo.flags = 0;
  queueInfo.queueFamilyIndex = queueFamily;
  queueInfo.queueCount = 1;
  queueInfo.pQueuePriorities = &queuePrio;

  // Create device
  VkDeviceCreateInfo devInfo;
  devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  devInfo.pNext = nullptr;
  devInfo.flags = 0;
  devInfo.queueCreateInfoCount = 1;
  devInfo.pQueueCreateInfos = &queueInfo;
  devInfo.enabledLayerCount = 0;
  devInfo.ppEnabledLayerNames = nullptr;
  devInfo.enabledExtensionCount = devExtensions_.size();
  devInfo.ppEnabledExtensionNames = devExtensions_.data();
  // TODO
  devInfo.pEnabledFeatures = nullptr;

  auto res = vkCreateDevice(physicalDev_, &devInfo, nullptr, &device_);
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Could not create logical device");

  // Now the queue object can be created
  setProcsVK(device_);
  VkQueue queue;
  vkGetDeviceQueue(device_, queueFamily, 0, &queue);
  queue_ = new QueueVK(queueFamily, queue);
}

VkInstance DeviceVK::instance() const {
  return instance_;
}

VkPhysicalDevice DeviceVK::physicalDev() const {
  return physicalDev_;
}

VkDevice DeviceVK::device() const {
  return device_;
}

const VkPhysicalDeviceProperties& DeviceVK::physProperties() const {
  return physProperties_;
}

const VkPhysicalDeviceMemoryProperties& DeviceVK::memProperties() const {
  return memProperties_;
}

const std::vector<const char*>& DeviceVK::instExtensions() const {
  return instExtensions_;
}

const std::vector<const char*>& DeviceVK::devExtensions() const {
  return devExtensions_;
}

const std::vector<const char*>& DeviceVK::layers() const {
  return layers_;
}

uint32_t DeviceVK::instVersion() const {
  return instVersion_;
}

uint32_t DeviceVK::devVersion() const {
  return physProperties_.apiVersion;
}

const VkPhysicalDeviceLimits& DeviceVK::limits() const {
  return physProperties_.limits;
}

Buffer::Ptr DeviceVK::makeBuffer(uint64_t size) {
  return make_unique<BufferVK>(size);
}

Image::Ptr DeviceVK::makeImage(PxFormat format,
                               Size2 size,
                               uint32_t layers,
                               uint32_t levels,
                               Samples samples) {
  // TODO
  assert(false);
}

Shader::Ptr DeviceVK::makeShader(Stage stage,
                                 wstring&& codeFile,
                                 wstring&& entryPoint) {

  return make_unique<ShaderVK>(stage, move(codeFile), move(entryPoint));
}

DcTable::Ptr DeviceVK::makeDcTable(const DcEntries& entries) {
  // TODO
  assert(false);
}

Pass::Ptr DeviceVK::makePass(const vector<ColorAttach>* colors,
                             const vector<ColorAttach>* resolves,
                             const DepStenAttach* depthStencil) {

  return make_unique<PassVK>(colors, resolves, depthStencil);
}

GrState::Ptr DeviceVK::makeState(const GrState::Config& config) {
  // TODO
  assert(false);
}

CpState::Ptr DeviceVK::makeState(const CpState::Config& config) {
  // TODO
  assert(false);
}

Queue& DeviceVK::defaultQueue() {
  return *queue_;
}

Queue& DeviceVK::queue(Queue::CapabilityMask) {
  return *queue_;
}
