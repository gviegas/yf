//
// yf
// DeviceVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cassert>
#include <unordered_set>
#include <string>

#include "DeviceVK.h"
#include "VK.h"

using namespace YF_NS;
using namespace std;

DeviceVK& DeviceVK::get() {
  static DeviceVK dev;
  return dev;
}

DeviceVK::DeviceVK() {
  if (!initVK())
    // TODO
    throw runtime_error("!initVK()");
  initInstance();
  initPhysicalDevice();
  initDevice();
}

CGResult DeviceVK::checkInstanceExtensions() {
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

  auto vkEnumerateInstanceExtensionProperties =
  YF_IPROCVK(nullptr, vkEnumerateInstanceExtensionProperties);

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

  return reqExts.empty() ? CGResult::Success : CGResult::Failure;
}

CGResult DeviceVK::checkDeviceExtensions() {
  _devExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  auto vkEnumerateDeviceExtensionProperties =
  YF_IPROCVK(_instance, vkEnumerateDeviceExtensionProperties);

  vector<VkExtensionProperties> exts;
  uint32_t extN;
  VkResult res;
  res = vkEnumerateDeviceExtensionProperties(_physicalDev, nullptr, &extN, nullptr);
  if (res == VK_SUCCESS && extN > 0) {
    exts.resize(extN);
    res = vkEnumerateDeviceExtensionProperties(_physicalDev, nullptr, &extN, exts.data());
    if (res != VK_SUCCESS)
      extN = 0;
  }

  unordered_set<string> reqExts;
  for (const auto& e : _devExtensions)
    reqExts.insert(e);

  for (const auto& e : exts)
    reqExts.erase(e.extensionName);

  return reqExts.empty() ? CGResult::Success : CGResult::Failure;
}

void DeviceVK::initInstance() {
  // Check extensions
  if (!checkInstanceExtensions())
    // TODO
    throw runtime_error("Missing required instance extensions");

  // Get instance version
  auto vkEnumerateInstanceVersion =
  YF_IPROCVK(nullptr, vkEnumerateInstanceVersion);
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

  auto vkCreateInstance = YF_IPROCVK(nullptr, vkCreateInstance);
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

  auto vkEnumeratePhysicalDevices =
  YF_IPROCVK(_instance, vkEnumeratePhysicalDevices);

  auto vkGetPhysicalDeviceProperties =
  YF_IPROCVK(_instance, vkGetPhysicalDeviceProperties);

  auto vkGetPhysicalDeviceQueueFamilyProperties =
  YF_IPROCVK(_instance, vkGetPhysicalDeviceQueueFamilyProperties);

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
    vkGetPhysicalDeviceQueueFamilyProperties(phys[p.first], &familyN, families.data());

    int32_t graph = -1;
    int32_t comp = -1;
    for (uint32_t i = 0; i < familyN; ++i) {
      if (families[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT|VK_QUEUE_COMPUTE_BIT)) {
        graph = comp = i;
        break;
      } else if (graph < 0 && (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
        graph = i;
      } else if (comp < 0 && (families[i].queueFlags & VK_QUEUE_COMPUTE_BIT)) {
        comp = i;
      }
    }

    // [1.2.146 c4.1]
    // "If an implementation exposes any queue family that supports graphics operations,
    // at least one queue family of at least one physical device exposed by the
    // implementation must support both graphics and compute operations."

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
  // TODO
}

CGDevice::BufferRes DeviceVK::makeBuffer(uint64_t size) {
  // TODO
  assert(false);
}

CGDevice::ImageRes DeviceVK::makeImage(CGPxFormat format,
                                       CGSize2 size,
                                       uint32_t layers,
                                       uint32_t levels,
                                       CGSamples samples) {
  // TODO
  assert(false);
}

CGDevice::ShaderRes DeviceVK::makeShader(CGStage stage,
                                         wstring&& codeFile,
                                         wstring&& entryPoint) {
  // TODO
  assert(false);
}

CGDevice::DcTableRes DeviceVK::makeDcTable(const CGDcEntries& entries) {
  // TODO
  assert(false);
}

CGDevice::DcTableRes DeviceVK::makeDcTable(CGDcEntries&& entries) {
  // TODO
  assert(false);
}

CGDevice::PassRes DeviceVK::makePass(const vector<CGColorAttach>* colors,
                                     const vector<CGColorAttach>* resolves,
                                     const CGDepStenAttach* depthStencil) {
  // TODO
  assert(false);
}

CGDevice::GrStateRes DeviceVK::makeState(const CGGrState::Config& config) {
  // TODO
  assert(false);
}

CGDevice::GrStateRes DeviceVK::makeState(CGGrState::Config&& config) {
  // TODO
  assert(false);
}

CGDevice::CpStateRes DeviceVK::makeState(const CGCpState::Config& config) {
  // TODO
  assert(false);
}

CGDevice::CpStateRes DeviceVK::makeState(CGCpState::Config&& config) {
  // TODO
  assert(false);
}

CGDevice::QueueRes DeviceVK::defaultQueue() {
  // TODO
  assert(false);
}

CGDevice::QueueRes DeviceVK::queue(CGQueue::CapabilityMask capabilities) {
  // TODO
  assert(false);
}
