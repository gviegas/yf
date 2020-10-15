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

// TODO
class DeviceVK::Impl {
 public:
  Impl() {
    if (!initVK()) {
      // TODO
      throw runtime_error("!initVK()");
    }
    initInstance();
    initDevice();
  }

  void checkInstanceExts() {
    _instExts.push_back(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME);
    _instExts.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(__linux__)
    _instExts.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
    _instExts.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(__APPLE__)
    _instExts.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
    _instExts.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
# error "Invalid platform"
#endif

    vector<VkExtensionProperties> exts;
    uint32_t extN;
    VkResult res;
    res = vk.enumerateInstanceExtensionProperties(nullptr, &extN, nullptr);
    if (res == VK_SUCCESS && extN > 0) {
      exts.resize(extN);
      res = vk.enumerateInstanceExtensionProperties(nullptr, &extN, exts.data());
      if (res != VK_SUCCESS)
        extN = 0;
    }

    unordered_set<string> reqExts;
    for (const auto& e : _instExts)
      reqExts.insert(e);

    for (const auto& e : exts)
      reqExts.erase(e.extensionName);

    if (!reqExts.empty())
      throw runtime_error("Missing required instance extensions");
  }

  void checkDeviceExts() {
    // TODO
  }

  void initInstance() {
    // Check extensions
    checkInstanceExts();

    // Get instance version
    if (!vk._1.enumerateInstanceVersion)
      _instVersion = VK_API_VERSION_1_0;
    else
      vk._1.enumerateInstanceVersion(&_instVersion);

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
    instInfo.enabledLayerCount = _instLayers.size();
    instInfo.ppEnabledLayerNames = _instLayers.data();
    instInfo.enabledExtensionCount = _instExts.size();
    instInfo.ppEnabledExtensionNames = _instExts.data();

    auto res = vk.createInstance(&instInfo, nullptr, &_instance);
    if (res != VK_SUCCESS)
      // TODO
      throw runtime_error("Failed to create VK instance");
  }

  void initDevice() {
  }

 private:
  // TODO
  VkInstance _instance;
  uint32_t _instVersion;
  vector<const char*> _instLayers;
  vector<const char*> _instExts;
};

DeviceVK::DeviceVK() : _impl(make_unique<Impl>()) {}

DeviceVK::~DeviceVK() {}

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
