//
// yf
// VK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_VK_H
#define YF_CG_VK_H

#include <vector>

#include "YFDefs.h"
#include "CGResult.h"

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

YF_NS_BEGIN

/// Initializes VK.
///
CGResult initVK();

/// Deinitializes VK.
///
void deinitVK();

// XXX: invalid until `initVK()` returns successfully
extern PFN_vkGetInstanceProcAddr getInstanceProcAddrVK;

#define YF_IPROCVK(instance, name) \
  reinterpret_cast<PFN_##name>(YF_NS::getInstanceProcAddrVK(instance, #name))

// XXX: invalid until `DeviceVK` object is created
extern PFN_vkGetDeviceProcAddr getDeviceProcAddrVK;

#define YF_DPROCVK(device, name) \
  reinterpret_cast<PFN_##name>(YF_NS::getDeviceProcAddrVK(device, #name))

YF_NS_END

#endif // YF_CG_VK_H
