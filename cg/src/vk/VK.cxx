//
// cg
// VK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "VK.h"
#include "yf/cg/Defs.h"

#if defined(__linux__)
# include <dlfcn.h>
# define CG_LIBVK "libvulkan.so.1"
#elif defined(__APPLE__)
# include <dlfcn.h>
# define CG_LIBVK "libvulkan.dylib"
#elif defined(_WIN32)
# include <windows.h>
# define CG_LIBVK "vulkan-1.dll"
#else
# error "Invalid platform"
#endif

using namespace CG_NS;

INTERNAL_NS_BEGIN

#if defined(__linux__) || defined(__APPLE__)
void* libHandle = nullptr;

inline Result loadVK() {
  if (libHandle)
    return Result::Success;

  void* handle = dlopen(CG_LIBVK, RTLD_LAZY);
  if (!handle)
    return Result::Failure;

  void* sym = dlsym(handle, "vkGetInstanceProcAddr");
  if (!sym) {
    dlclose(handle);
    return Result::Failure;
  }

  libHandle = handle;
  getInstanceProcAddrVK = reinterpret_cast<PFN_vkGetInstanceProcAddr>(sym);

  return Result::Success;
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

PFN_vkGetInstanceProcAddr CG_NS::getInstanceProcAddrVK;
PFN_vkGetDeviceProcAddr CG_NS::getDeviceProcAddrVK;

Result CG_NS::initVK() {
  return loadVK();
}

void CG_NS::deinitVK() {
  unloadVK();
}
