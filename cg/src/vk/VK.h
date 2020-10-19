//
// cg
// VK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_VK_H
#define YF_CG_VK_H

#include <vector>

#include "yf/cg/Defs.h"
#include "yf/cg/Result.h"

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

CG_NS_BEGIN

/// Initializes VK.
///
Result initVK();

/// Deinitializes VK.
///
void deinitVK();

// XXX: invalid until `initVK()` returns successfully
extern PFN_vkGetInstanceProcAddr getInstanceProcAddrVK;

#define CG_INSTPROCVK_RVAL(instance, name) \
reinterpret_cast<PFN_##name>(CG_NS::getInstanceProcAddrVK(instance, #name))

#define CG_INSTPROCVK(instance, name) \
PFN_##name name = CG_INSTPROCVK_RVAL(instance, name)

// XXX: invalid until `DeviceVK` object is created
extern PFN_vkGetDeviceProcAddr getDeviceProcAddrVK;

#define CG_DEVPROCVK_RVAL(device, name) \
reinterpret_cast<PFN_##name>(CG_NS::getDeviceProcAddrVK(device, #name))

#define CG_DEVPROCVK(device, name) \
PFN_##name name = CG_DEVPROCVK_RVAL(device, name)

CG_NS_END

#endif // YF_CG_VK_H
