//
// yf
// VK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_VK_H
#define YF_CG_VK_H

#if defined(__linux__)
# define VK_USE_PLATFORM_WAYLAND_KHR
# define VK_USE_PLATFORM_XCB_KHR
#elif defined(__APPLE__)
# define VK_USE_PLATFORM_METAL_EXT
#elif defined (_WIN32)
# define VK_USE_PLATFORM_WIN32_KHR
#else
# error "Invalid platform"
#endif
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "YFDefs.h"
#include "CGResult.h"

YF_NS_BEGIN

/// Loads VK lib and sets object-independent procedures of `vk` structure.
///
CGResult initVK();

/// Sets instance-level procedures of `vk` structure.
///
CGResult setInstanceVK(VkInstance instance);

/// Sets device-level procedures of `vk` structure.
///
CGResult setDeviceVK(VkDevice device);

/// Unloads VK lib and clears `vk` structure.
///
void deinitVK();

/// VK procedures (v1.x).
///
struct VK1 {
  PFN_vkGetInstanceProcAddr getInstanceProcAddr;
  PFN_vkGetDeviceProcAddr getDeviceProcAddr;

  PFN_vkEnumerateInstanceExtensionProperties enumerateInstanceExtensionProperties;
  PFN_vkEnumerateInstanceLayerProperties enumerateInstanceLayerProperties;
  PFN_vkCreateInstance createInstance;

  PFN_vkDestroyInstance destroyInstance;
  PFN_vkEnumeratePhysicalDevices enumeratePhysicalDevices;
  PFN_vkGetPhysicalDeviceProperties getPhysicalDeviceProperties;
  PFN_vkGetPhysicalDeviceQueueFamilyProperties getPhysicalDeviceQueueFamilyProperties;
  PFN_vkCreateDevice createDevice;

  PFN_vkDestroyDevice destroyDevice;
  PFN_vkGetDeviceQueue getDeviceQueue;

  /// v1.1.
  ///
  struct {
    PFN_vkEnumerateInstanceVersion enumerateInstanceVersion;

    PFN_vkEnumeratePhysicalDeviceGroups enumeratePhysicalDeviceGroups;
  } _1;

  /// v1.2.
  ///
  struct {
  } _2;

  /// Extensions.
  ///
  struct {
  } ext;
};
static_assert(std::is_pod<VK1>(), "!is_pod<VK1>");

extern VK1 vk;

YF_NS_END

#endif // YF_CG_VK_H
