//
// CG
// DeviceVK.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cassert>
#include <unordered_set>
#include <string>
#include <algorithm>

#include "DeviceVK.h"
#include "VK.h"
#include "QueueVK.h"
#include "BufferVK.h"
#include "ImageVK.h"
#include "ShaderVK.h"
#include "DcTableVK.h"
#include "PassVK.h"
#include "StateVK.h"
#include "WsiVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

DeviceVK& CG_NS::deviceVK() {
  static DeviceVK dev;
  return dev;
}

DeviceVK::DeviceVK() {
  initVK();
  initInstance();
}

DeviceVK::~DeviceVK() {
  if (device_ != nullptr) {
    vkDeviceWaitIdle(device_);
    vkDestroyPipelineCache(device_, cache_, nullptr);
    // TODO: ensure that all VK objects were disposed of prior to this point
    delete queue_;
    vkDestroyDevice(device_, nullptr);
  }
  vkDestroyInstance(instance_, nullptr);
  deinitVK();
}

bool DeviceVK::checkInstanceExtensions() {
  assert(instExtensions_.empty());

  setProcsVK(static_cast<VkInstance>(nullptr));

  // Set required extensions
  instExtensions_.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
  instExtensions_.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
  instExtensions_.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
  instExtensions_.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
  instExtensions_.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
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
    throw UnsupportedExcept("Missing required instance extension(s)");

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
    throw DeviceExcept("Failed to create VK instance");

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
    throw DeviceExcept("Could not enumerate physical devices");
  phys.resize(physN);
  res = vkEnumeratePhysicalDevices(instance_, &physN, phys.data());
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not enumerate physical devices");

  vector<pair<uint32_t, VkPhysicalDeviceProperties>> physProps;
  physProps.resize(physN);
  for (uint32_t i = 0; i < physN; i++) {
    physProps[i].first = i;
    vkGetPhysicalDeviceProperties(phys[i], &physProps[i].second);
  }

  // FIXME: too many assumptions
  sort(physProps.begin(), physProps.end(), [](auto& p1, auto& p2) {
    const auto t1 = p1.second.deviceType;
    const auto t2 = p2.second.deviceType;
    const auto v1 = p1.second.apiVersion;
    const auto v2 = p2.second.apiVersion;
    decltype(t1) types[]{VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                         VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
                         VK_PHYSICAL_DEVICE_TYPE_CPU};
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
  int32_t presFamily = -1;
  for (const auto& p : physProps) {
    vector<VkQueueFamilyProperties> families;
    uint32_t familyN;
    vkGetPhysicalDeviceQueueFamilyProperties(phys[p.first], &familyN, nullptr);
    families.resize(familyN);
    vkGetPhysicalDeviceQueueFamilyProperties(phys[p.first], &familyN,
                                             families.data());

    for (uint32_t i = 0; i < familyN; i++) {
      if (families[i].queueFlags &
      (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) {
        queueFamily = i;
        break;
      }
    }

    // [1.2.166 c5.1]
    // "If an implementation exposes any queue family that supports graphics
    // operations, at least one queue family of at least one physical device
    // exposed by the implementation must support both graphics and compute
    // operations."

    if (queueFamily > -1) {
      physicalDev_ = phys[p.first];
      physProperties_ = p.second;

      // Find a queue family on this device that supports presentation
      for (uint32_t i = 0; i < familyN; i++) {
        auto nextFamily = (queueFamily + i) % familyN;
        if (WsiVK::checkPhysicalDevice(physicalDev_, nextFamily)) {
          presFamily = nextFamily;
          break;
        }
      }

      break;
    }
  }

  if (physicalDev_ == nullptr)
    throw UnsupportedExcept("Could not find a suitable physical device");

  vkGetPhysicalDeviceMemoryProperties(physicalDev_, &memProperties_);
  setLimits();

  // Now the logical device can be initialized
  initDevice(queueFamily, presFamily);
}

void DeviceVK::initDevice(int32_t queueFamily, int32_t presFamily) {
  assert(queueFamily > -1);
  assert(physicalDev_ != nullptr);
  assert(device_ == nullptr);

  // Check extensions
  if (!checkDeviceExtensions())
    throw UnsupportedExcept("Missing required device extension(s)");

  // Define queues
  const float queuePrio = 0.0f;
  vector<VkDeviceQueueCreateInfo> queueInfos{1};
  queueInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueInfos[0].pNext = nullptr;
  queueInfos[0].flags = 0;
  queueInfos[0].queueFamilyIndex = queueFamily;
  queueInfos[0].queueCount = 1;
  queueInfos[0].pQueuePriorities = &queuePrio;

  if (presFamily > -1 && presFamily != queueFamily) {
    queueInfos.push_back(queueInfos[0]);
    queueInfos[1].queueFamilyIndex = presFamily;
  }

  // Create device
  VkDeviceCreateInfo devInfo;
  devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  devInfo.pNext = nullptr;
  devInfo.flags = 0;
  devInfo.queueCreateInfoCount = queueInfos.size();
  devInfo.pQueueCreateInfos = queueInfos.data();
  devInfo.enabledLayerCount = 0;
  devInfo.ppEnabledLayerNames = nullptr;
  devInfo.enabledExtensionCount = devExtensions_.size();
  devInfo.ppEnabledExtensionNames = devExtensions_.data();
  // TODO
  devInfo.pEnabledFeatures = nullptr;

  auto res = vkCreateDevice(physicalDev_, &devInfo, nullptr, &device_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create logical device");

  // Now the queue object can be created
  setProcsVK(device_);
  VkQueue queue;
  vkGetDeviceQueue(device_, queueFamily, 0, &queue);
  queue_ = new QueueVK(queue, queueFamily);
  if (presFamily < 0) {
    // Cannot present
    WsiVK::setQueue(nullptr, -1);
  } else {
    if (presFamily != queueFamily)
      vkGetDeviceQueue(device_, presFamily, 0, &queue);
    WsiVK::setQueue(queue, presFamily);
  }

  // Use a single cache for state creation
  VkPipelineCacheCreateInfo cacheInfo;
  cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  cacheInfo.pNext = nullptr;
  cacheInfo.flags = 0;
  cacheInfo.initialDataSize = 0;
  cacheInfo.pInitialData = nullptr;

  res = vkCreatePipelineCache(device_, &cacheInfo, nullptr, &cache_);
  if (res != VK_SUCCESS)
    cache_ = VK_NULL_HANDLE;
}

void DeviceVK::setFeatures() {
  assert(physicalDev_);

  // TODO: other v1.0 features & features from new versions

  decltype(features_) feat;
  vkGetPhysicalDeviceFeatures(physicalDev_, &feat);

  features_.geometryShader = feat.geometryShader;
  features_.tessellationShader = feat.tessellationShader;
  features_.fillModeNonSolid = feat.fillModeNonSolid;
  features_.wideLines = feat.wideLines;
  features_.largePoints = feat.largePoints;
  features_.multiViewport = feat.multiViewport;
#ifdef YF_DEVEL
  features_.occlusionQueryPrecise = feat.occlusionQueryPrecise;
  features_.pipelineStatisticsQuery = feat.pipelineStatisticsQuery;
#endif
  features_.vertexPipelineStoresAndAtomics =
    feat.vertexPipelineStoresAndAtomics;
  features_.fragmentStoresAndAtomics = feat.fragmentStoresAndAtomics;
  features_.shaderClipDistance = feat.shaderClipDistance;
  features_.shaderCullDistance = feat.shaderCullDistance;
}

void DeviceVK::setLimits() {
  assert(physicalDev_);

  const auto& lim = physProperties_.limits;

  limits_.maxDrawIndex = lim.maxDrawIndexedIndexValue;
  limits_.maxDispatchWidth = lim.maxComputeWorkGroupCount[0];
  limits_.maxDispatchHeight = lim.maxComputeWorkGroupCount[1];
  limits_.maxDispatchDepth = lim.maxComputeWorkGroupCount[2];

  limits_.maxViewports = lim.maxViewports;
  limits_.maxViewportWidth = lim.maxViewportDimensions[0];
  limits_.maxViewportHeight = lim.maxViewportDimensions[1];

  limits_.maxImageWidth = lim.maxImageDimension2D;
  limits_.maxImageHeight = lim.maxImageDimension2D;
  limits_.maxImageLayers = lim.maxImageArrayLayers;

  limits_.maxPassColors = lim.maxColorAttachments;
  limits_.maxTargetWidth = lim.maxFramebufferWidth;
  limits_.maxTargetHeight = lim.maxFramebufferHeight;
  limits_.maxTargetLayers = lim.maxFramebufferLayers;

  limits_.maxDcUniform = lim.maxDescriptorSetUniformBuffers;
  limits_.maxDcStorage = lim.maxDescriptorSetStorageBuffers;
  limits_.maxDcImage = lim.maxDescriptorSetStorageImages;
  limits_.maxDcImgSampler = min(lim.maxDescriptorSetSamplers,
                                lim.maxDescriptorSetSampledImages);
  limits_.minDcUniformWriteAlignedOffset = lim.minUniformBufferOffsetAlignment;
  limits_.maxDcUniformWriteSize = lim.maxUniformBufferRange;
  limits_.minDcStorageWriteAlignedOffset = lim.minStorageBufferOffsetAlignment;
  limits_.maxDcStorageWriteSize = lim.maxStorageBufferRange;

  limits_.maxVxInputs = lim.maxVertexInputBindings;
  limits_.maxVxAttrs = lim.maxVertexInputAttributes;
}

VkInstance DeviceVK::instance() {
  return instance_;
}

VkPhysicalDevice DeviceVK::physicalDev() {
  return physicalDev_;
}

VkDevice DeviceVK::device() {
  return device_;
}

VkPipelineCache DeviceVK::cache() {
  return cache_;
}

const VkPhysicalDeviceProperties& DeviceVK::physProperties() const {
  return physProperties_;
}

const VkPhysicalDeviceMemoryProperties& DeviceVK::memProperties() const {
  return memProperties_;
}

const vector<const char*>& DeviceVK::instExtensions() const {
  return instExtensions_;
}

const vector<const char*>& DeviceVK::devExtensions() const {
  return devExtensions_;
}

const vector<const char*>& DeviceVK::layers() const {
  return layers_;
}

uint32_t DeviceVK::instVersion() const {
  return instVersion_;
}

uint32_t DeviceVK::devVersion() const {
  return physProperties_.apiVersion;
}

const VkPhysicalDeviceLimits& DeviceVK::physLimits() const {
  return physProperties_.limits;
}

Queue& DeviceVK::defaultQueue() {
  return *queue_;
}

Queue& DeviceVK::queue(Queue::CapabilityMask) {
  return *queue_;
}

Buffer::Ptr DeviceVK::buffer(uint64_t size) {
  return make_unique<BufferVK>(size);
}

Image::Ptr DeviceVK::image(PxFormat format, Size2 size, uint32_t layers,
                           uint32_t levels, Samples samples) {

  return make_unique<ImageVK>(format, size, layers, levels, samples);
}

Shader::Ptr DeviceVK::shader(Stage stage, const wstring& codeFile,
                             const wstring& entryPoint) {

  return make_unique<ShaderVK>(stage, codeFile, entryPoint);
}

DcTable::Ptr DeviceVK::dcTable(const DcEntries& entries) {
  return make_unique<DcTableVK>(entries);
}

Pass::Ptr DeviceVK::pass(const vector<ColorAttach>* colors,
                         const vector<ColorAttach>* resolves,
                         const DepStenAttach* depthStencil) {

  return make_unique<PassVK>(colors, resolves, depthStencil);
}

GrState::Ptr DeviceVK::state(const GrState::Config& config) {
  return make_unique<GrStateVK>(config);
}

CpState::Ptr DeviceVK::state(const CpState::Config& config) {
  return make_unique<CpStateVK>(config);
}

Wsi::Ptr DeviceVK::wsi(WS_NS::Window* window) {
  return make_unique<WsiVK>(window);
}

const Limits& DeviceVK::limits() const {
  return limits_;
}
