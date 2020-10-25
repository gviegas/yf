//
// cg
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
  if (_device != nullptr) {
    CG_DEVPROCVK(_device, vkDeviceWaitIdle);
    CG_DEVPROCVK(_device, vkDestroyDevice);
    vkDeviceWaitIdle(_device);
    // TODO: ensure that all VK objects were disposed of prior to this point
    delete _queue;
    vkDestroyDevice(_device, nullptr);
  }
  if (_instance != nullptr) {
    CG_INSTPROCVK(_instance, vkDestroyInstance);
    vkDestroyInstance(_instance, nullptr);
  }
}

Result DeviceVK::checkInstanceExtensions() {
  assert(_instExtensions.empty());

  // Define required extensions
  _instExtensions.push_back(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME);
  _instExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(__linux__)
  _instExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
  _instExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(__APPLE__)
  _instExtensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
  _instExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
# error "Invalid platform"
#endif

  // Enumerate the ones that the instance offers
  CG_INSTPROCVK(nullptr, vkEnumerateInstanceExtensionProperties);
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
  for (const auto& e : _instExtensions)
    reqExts.insert(e);
  for (const auto& e : exts)
    reqExts.erase(e.extensionName);

  return reqExts.empty() ? Result::Success : Result::Failure;
}

Result DeviceVK::checkDeviceExtensions() {
  assert(_physicalDev != nullptr);
  assert(_devExtensions.empty());

  // Define required extensions (just this one for now)
  _devExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  // Enumerate the ones that the device offers
  CG_INSTPROCVK(_instance, vkEnumerateDeviceExtensionProperties);
  vector<VkExtensionProperties> exts;
  uint32_t extN;
  VkResult res;
  res = vkEnumerateDeviceExtensionProperties(_physicalDev, nullptr, &extN,
                                             nullptr);
  if (res == VK_SUCCESS && extN > 0) {
    exts.resize(extN);
    res = vkEnumerateDeviceExtensionProperties(_physicalDev, nullptr, &extN,
                                               exts.data());
    if (res != VK_SUCCESS)
      extN = 0;
  }

  // Check if the device has everything
  unordered_set<string> reqExts;
  for (const auto& e : _devExtensions)
    reqExts.insert(e);
  for (const auto& e : exts)
    reqExts.erase(e.extensionName);

  return reqExts.empty() ? Result::Success : Result::Failure;
}

void DeviceVK::initInstance() {
  assert(getInstanceProcAddrVK != nullptr);
  assert(_instance == nullptr);

  // Check extensions
  if (!checkInstanceExtensions())
    // TODO
    throw runtime_error("Missing required instance extension(s)");

  // Get instance version
  CG_INSTPROCVK(nullptr, vkEnumerateInstanceVersion);
  if (!vkEnumerateInstanceVersion)
    _instVersion = VK_API_VERSION_1_0;
  else
    vkEnumerateInstanceVersion(&_instVersion);

  // Create instance
  VkApplicationInfo appInfo;
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pNext = nullptr;
  appInfo.pApplicationName = "yf";
  appInfo.applicationVersion = 0;
  appInfo.pEngineName = nullptr;
  appInfo.engineVersion = 0;
  appInfo.apiVersion = _instVersion;

  VkInstanceCreateInfo instInfo;
  instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instInfo.pNext = nullptr;
  instInfo.flags = 0;
  instInfo.pApplicationInfo = &appInfo;
  instInfo.enabledLayerCount = _layers.size();
  instInfo.ppEnabledLayerNames = _layers.data();
  instInfo.enabledExtensionCount = _instExtensions.size();
  instInfo.ppEnabledExtensionNames = _instExtensions.data();

  CG_INSTPROCVK(nullptr, vkCreateInstance);
  auto res = vkCreateInstance(&instInfo, nullptr, &_instance);
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Failed to create VK instance");

  // Now the physical device can be initialized
  initPhysicalDevice();
}

void DeviceVK::initPhysicalDevice() {
  assert(_instance != nullptr);
  assert(_physicalDev == nullptr);

  VkResult res;

  CG_INSTPROCVK(_instance, vkEnumeratePhysicalDevices);
  CG_INSTPROCVK(_instance, vkGetPhysicalDeviceProperties);
  CG_INSTPROCVK(_instance, vkGetPhysicalDeviceQueueFamilyProperties);

  // Enumerate physical devices & get their properties
  vector<VkPhysicalDevice> phys;
  uint32_t physN;
  res = vkEnumeratePhysicalDevices(_instance, &physN, nullptr);
  if (res != VK_SUCCESS || physN == 0)
    // TODO
    throw runtime_error("Could not enumerate physical devices");
  phys.resize(physN);
  res = vkEnumeratePhysicalDevices(_instance, &physN, phys.data());
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
      _physicalDev = phys[p.first];
      _physProperties = p.second;
      break;
    }
  }

  if (_physicalDev == nullptr)
    // TODO
    throw runtime_error("Could not find a suitable physical device");

  // Now the logical device can be initialized
  initDevice(queueFamily);
}

void DeviceVK::initDevice(int32_t queueFamily) {
  assert(queueFamily > -1);
  assert(_physicalDev != nullptr);
  assert(_device == nullptr);

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
  devInfo.enabledExtensionCount = _devExtensions.size();
  devInfo.ppEnabledExtensionNames = _devExtensions.data();
  // TODO
  devInfo.pEnabledFeatures = nullptr;

  CG_INSTPROCVK(_instance, vkCreateDevice);
  auto res = vkCreateDevice(_physicalDev, &devInfo, nullptr, &_device);
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Could not create logical device");

  // Set `getDeviceProcAddrVK` with the newly created device object
  getDeviceProcAddrVK =
  reinterpret_cast<PFN_vkGetDeviceProcAddr>
  (getInstanceProcAddrVK(_instance, "vkGetDeviceProcAddr"));

  // Now the queue object can be created
  CG_DEVPROCVK(_device, vkGetDeviceQueue);
  VkQueue queue;
  vkGetDeviceQueue(_device, queueFamily, 0, &queue);
  _queue = new QueueVK(queueFamily, queue);
  _queue->setProcs(_device, _physProperties.apiVersion);
}

VkInstance DeviceVK::instance() const {
  return _instance;
}
VkPhysicalDevice DeviceVK::physicalDev() const {
  return _physicalDev;
}
VkDevice DeviceVK::device() const {
  return _device;
}
const VkPhysicalDeviceProperties& DeviceVK::physProperties() const {
  return _physProperties;
}
const std::vector<const char*>& DeviceVK::instExtensions() const {
  return _instExtensions;
}
const std::vector<const char*>& DeviceVK::devExtensions() const {
  return _devExtensions;
}
const std::vector<const char*>& DeviceVK::layers() const {
  return _layers;
}
uint32_t DeviceVK::instVersion() const {
  return _instVersion;
}
uint32_t DeviceVK::devVersion() const {
  return _physProperties.apiVersion;
}
const VkPhysicalDeviceLimits& DeviceVK::limits() const {
  return _physProperties.limits;
}

Buffer::Ptr DeviceVK::makeBuffer(uint64_t size) {
  // TODO
  assert(false);
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
  // TODO
  assert(false);
}

DcTable::Ptr DeviceVK::makeDcTable(const DcEntries& entries) {
  // TODO
  assert(false);
}

DcTable::Ptr DeviceVK::makeDcTable(DcEntries&& entries) {
  // TODO
  assert(false);
}

Pass::Ptr DeviceVK::makePass(const vector<ColorAttach>* colors,
                             const vector<ColorAttach>* resolves,
                             const DepStenAttach* depthStencil) {
  // TODO
  assert(false);
}

GrState::Ptr DeviceVK::makeState(const GrState::Config& config) {
  // TODO
  assert(false);
}

GrState::Ptr DeviceVK::makeState(GrState::Config&& config) {
  // TODO
  assert(false);
}

CpState::Ptr DeviceVK::makeState(const CpState::Config& config) {
  // TODO
  assert(false);
}

CpState::Ptr DeviceVK::makeState(CpState::Config&& config) {
  // TODO
  assert(false);
}

Queue& DeviceVK::defaultQueue() {
  return *_queue;
}

Queue& DeviceVK::queue(Queue::CapabilityMask) {
  return *_queue;
}
