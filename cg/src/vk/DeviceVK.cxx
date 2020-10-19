//
// cg
// DeviceVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cassert>
#include <unordered_set>
#include <string>

#include "DeviceVK.h"
#include "VK.h"

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
  initPhysicalDevice();
  initDevice();
}

DeviceVK::~DeviceVK() {
  // TODO: ensure that all VK objects were disposed of prior to this point
  if (_device != nullptr) {
    CG_DEVPROCVK(_device, vkDeviceWaitIdle);
    CG_DEVPROCVK(_device, vkDestroyDevice);
    vkDeviceWaitIdle(_device);
    vkDestroyDevice(_device, nullptr);
  }
  if (_instance != nullptr) {
    CG_INSTPROCVK(_instance, vkDestroyInstance);
    vkDestroyInstance(_instance, nullptr);
  }
}

Result DeviceVK::checkInstanceExtensions() {
  assert(_instExtensions.empty());

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

  _devExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

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
}

void DeviceVK::initPhysicalDevice() {
  assert(_instance != nullptr);
  assert(_physicalDev == nullptr);
  assert(_graphFamily == -1 && _compFamily == -1);

  VkResult res;

  CG_INSTPROCVK(_instance, vkEnumeratePhysicalDevices);
  CG_INSTPROCVK(_instance, vkGetPhysicalDeviceProperties);
  CG_INSTPROCVK(_instance, vkGetPhysicalDeviceQueueFamilyProperties);

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

  for (const auto& p : physProps) {
    vector<VkQueueFamilyProperties> families;
    uint32_t familyN;
    vkGetPhysicalDeviceQueueFamilyProperties(phys[p.first], &familyN, nullptr);
    families.resize(familyN);
    vkGetPhysicalDeviceQueueFamilyProperties(phys[p.first], &familyN,
                                             families.data());

    int32_t graph = -1;
    int32_t comp = -1;
    for (uint32_t i = 0; i < familyN; ++i) {
      if (families[i].queueFlags &
      (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) {
        graph = comp = i;
        break;
      } else if (graph < 0 &&
      (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
        graph = i;
      } else if (comp < 0 &&
      (families[i].queueFlags & VK_QUEUE_COMPUTE_BIT)) {
        comp = i;
      }
    }

    // [1.2.146 c4.1]
    // "If an implementation exposes any queue family that supports graphics
    // operations, at least one queue family of at least one physical device
    // exposed by the implementation must support both graphics and compute
    // operations."

    if (graph > -1 && comp > -1) {
      _physicalDev = phys[p.first];
      _physProperties = p.second;
      _graphFamily = graph;
      _compFamily = comp;
      break;
    }
  }

  if (_physicalDev == nullptr)
    // TODO
    throw runtime_error("Could not find a suitable physical device");
}

void DeviceVK::initDevice() {
  assert(_physicalDev != nullptr);
  assert(_graphFamily > -1);
  assert(_device == nullptr);

  // Check extensions
  if (!checkDeviceExtensions())
    // TODO
    throw runtime_error("Missing required device extension(s)");

  // Define queues
  const float queuePrio = 0.0f;
  VkDeviceQueueCreateInfo queueInfos[2];
  queueInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueInfos[0].pNext = nullptr;
  queueInfos[0].flags = 0;
  queueInfos[0].queueFamilyIndex = _graphFamily;
  queueInfos[0].queueCount = 1;
  queueInfos[0].pQueuePriorities = &queuePrio;
  uint32_t queueN = 1;
  if (_graphFamily != _compFamily) {
    queueInfos[1] = queueInfos[0];
    queueInfos[1].queueFamilyIndex = _compFamily;
    queueN = 2;
  }

  // Create device
  VkDeviceCreateInfo devInfo;
  devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  devInfo.pNext = nullptr;
  devInfo.flags = 0;
  devInfo.queueCreateInfoCount = queueN;
  devInfo.pQueueCreateInfos = queueInfos;
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

  // Get queues
  CG_DEVPROCVK(_device, vkGetDeviceQueue);
  vkGetDeviceQueue(_device, _graphFamily, 0, &_graphQueue);
  vkGetDeviceQueue(_device, _compFamily, 0, &_compQueue);
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
  // TODO
  assert(false);
}

Queue& DeviceVK::queue(Queue::CapabilityMask capabilities) {
  // TODO
  assert(false);
}
