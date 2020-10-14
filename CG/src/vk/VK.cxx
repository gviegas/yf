//
// yf
// VK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifdef __linux__
# include <dlfcn.h>
#else
# error "Invalid platform" // TODO
#endif

#include <cstring>

#include "VK.h"
#include "Defs.h"

using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

/// Loads VK lib and sets `vk.getInstanceProcAddr`.
///
CGResult loadVK();

/// Unloads VK lib.
///
void unloadVK();

#ifdef __linux__
void* libHandle = nullptr;

CGResult loadVK() {
  if (libHandle)
    return CGResult::Success;

  const char libName[] = "libvulkan.so.1";
  void* handle = dlopen(libName, RTLD_LAZY);
  if (!handle)
    return CGResult::Failure;

  const char fnName[] = "vkGetInstanceProcAddr";
  void* sym = dlsym(handle, fnName);
  if (!sym) {
    dlclose(handle);
    return CGResult::Failure;
  }

  libHandle = handle;
  vk.getInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(sym);

  return CGResult::Success;
}

void unloadVK() {
  if (libHandle) {
    dlclose(libHandle);
    libHandle = nullptr;
  }
}
#else
# error "Invalid platform" // TODO
#endif

INTERNAL_NS_END

#define YF_IPROCVK(instance, name) \
  reinterpret_cast<PFN_##name>(vk.getInstanceProcAddr(instance, #name))
#define YF_DPROCVK(device, name) \
  reinterpret_cast<PFN_##name>(vk.getDeviceProcAddr(device, #name))

VK1 YF_NS::vk;

CGResult YF_NS::initVK() {
  if (!loadVK())
    return CGResult::Failure;

  vk.enumerateInstanceExtensionProperties =
  YF_IPROCVK(nullptr, vkEnumerateInstanceExtensionProperties);

  vk.enumerateInstanceLayerProperties =
  YF_IPROCVK(nullptr, vkEnumerateInstanceLayerProperties);

  vk.createInstance = YF_IPROCVK(nullptr, vkCreateInstance);

  vk._1.enumerateInstanceVersion =
  YF_IPROCVK(nullptr, vkEnumerateInstanceVersion);

  return CGResult::Success;
}

CGResult YF_NS::setInstanceVK(VkInstance instance) {
  if (!vk.getInstanceProcAddr || !instance)
    return CGResult::Failure;

  return CGResult::Success;

  // TODO
}

CGResult YF_NS::setDeviceVK(VkDevice device) {
  if (!vk.getDeviceProcAddr || !device)
    return CGResult::Failure;

  return CGResult::Success;

  // TODO
}

void YF_NS::deinitVK() {
  memset(&vk, 0, sizeof vk);
  unloadVK();
}
