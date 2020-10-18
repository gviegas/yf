//
// yf
// VK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "VK.h"
#include "Defs.h"

#if defined(__linux__)
# include <dlfcn.h>
# define YF_LIBVK "libvulkan.so.1"
#elif defined(__APPLE__)
# include <dlfcn.h>
# define YF_LIBVK "libvulkan.dylib"
#elif defined(_WIN32)
# include <windows.h>
# define YF_LIBVK "vulkan-1.dll"
#else
# error "Invalid platform"
#endif

using namespace YF_NS;

INTERNAL_NS_BEGIN

#if defined(__linux__) || defined(__APPLE__)
void* libHandle = nullptr;

inline CGResult loadVK() {
  if (libHandle)
    return CGResult::Success;

  void* handle = dlopen(YF_LIBVK, RTLD_LAZY);
  if (!handle)
    return CGResult::Failure;

  void* sym = dlsym(handle, "vkGetInstanceProcAddr");
  if (!sym) {
    dlclose(handle);
    return CGResult::Failure;
  }

  libHandle = handle;
  getInstanceProcAddrVK = reinterpret_cast<PFN_vkGetInstanceProcAddr>(sym);

  return CGResult::Success;
}

inline void unloadVK() {
  if (libHandle) {
    dlclose(libHandle);
    libHandle = nullptr;
  }
}
#elif defined(_WIN32)
# error "Unimplemented"
#else
# error "Invalid platform"
#endif // defined(__linux__) || defined(__APPLE__)

INTERNAL_NS_END

PFN_vkGetInstanceProcAddr YF_NS::getInstanceProcAddrVK;
PFN_vkGetDeviceProcAddr YF_NS::getDeviceProcAddrVK;

CGResult YF_NS::initVK() {
  return loadVK();
}

void YF_NS::deinitVK() {
  unloadVK();
}
