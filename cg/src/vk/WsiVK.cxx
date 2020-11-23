//
// CG
// WsiVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "WsiVK.h"
#include "yf/ws/Platform.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

bool WsiVK::checkPhysicalDevice(VkPhysicalDevice physicalDev, int32_t family) {
  auto pfm = WS_NS::platform();
  VkBool32 r;

#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
// Wayland and XCB
# if defined(VK_USE_PLATFORM_XCB_KHR)
  switch (pfm) {
  case WS_NS::PlatformWL:
    // TODO
    throw runtime_error("Unimplemented");
    break;
  case WS_NS::PlatformXCB:
    r = vkGetPhysicalDeviceXcbPresentationSupportKHR(physicalDev, family,
                                                     WS_NS::connectionXCB(),
                                                     WS_NS::visualIdXCB());
    break;
  default:
    throw runtime_error("Platform mismatch");
  }
// Wayland only
# else
  if (platform != WS_NS::PlatformWL)
    throw runtime_error("Platform mismatch");

  // TODO
  throw runtime_error("Unimplemented");
# endif

// Win32
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
  if (pfm != WS_NS::PlatformW32)
    throw runtime_error("Platform mismatch");

  // TODO
  throw runtime_error("Unimplemented");

// XCB
#elif defined(VK_USE_PLATFORM_XCB_KHR)
  if (pfm != WS_NS::PlatformXCB)
    throw runtime_error("Platform mismatch");

  r = vkGetPhysicalDeviceXcbPresentationSupportKHR(physicalDev, family,
                                                   WS_NS::connectionXCB(),
                                                   WS_NS::visualIdXCB());

// CAMetalLayer
#elif defined(VK_USE_PLATFORM_METAL_EXT)
  if (pfm != WS_NS::PlatformMAC)
    throw runtime_error("Platform mismatch");

  // TODO
  throw runtime_error("Unimplemented");

#else
  return false;
#endif // defined(VK_USE_PLATFORM_WAYLAND_KHR)

  return r == VK_TRUE;
}

void WsiVK::setQueue(VkQueue queue, int32_t family) {
  queue_ = queue;
  family_ = family;
}

VkQueue WsiVK::queue_ = nullptr;
int32_t WsiVK::family_ = -1;

WsiVK::WsiVK(WS_NS::Window* window) : Wsi(window) {
  if (!window_)
    throw invalid_argument("WsiVK requires a valid window object");

  // TODO
  throw runtime_error("Unimplemented");
}

WsiVK::~WsiVK() {
  // TODO
}

const vector<Image*>& WsiVK::images() const {
  return images_;
}

Image* WsiVK::nextImage() {
  // TODO
  throw runtime_error("Unimplemented");
}

void WsiVK::present() {
  // TODO
  throw runtime_error("Unimplemented");
}
