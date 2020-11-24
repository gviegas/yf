//
// CG
// WsiVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cassert>
#include <array>

#include "WsiVK.h"
#include "DeviceVK.h"
#include "ImageVK.h"
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
  if (!queue_ || family_ < 0)
    throw UnsupportedExcept("Wsi not supported");

  initSurface();
  initSwapchain();
}

WsiVK::~WsiVK() {
  // TODO
}

void WsiVK::initSurface() {
  assert(surface_ == VK_NULL_HANDLE);

  auto pfm = WS_NS::platform();
  auto inst = DeviceVK::get().instance();
  VkResult res;

#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
// Wayland and XCB
# if defined(VK_USE_PLATFORM_XCB_KHR)
  switch (pfm) {
  case WS_NS::PlatformWL:
    // TODO
    throw runtime_error("Unimplemented");
    break;
  case WS_NS::PlatformXCB: {
    VkXcbSurfaceCreateInfoKHR info{
      VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
      nullptr,
      0,
      WS_NS::connectionXCB(),
      WS_NS::windowXCB(window_)
    };
    res = vkCreateXcbSurfaceKHR(inst, &info, nullptr, &surface_);
    if (res != VK_SUCCESS)
      throw DeviceExcept("Could not create XCB surface");
    } break;
  default:
    throw runtime_error("Platform mismatch");
  }
// Wayland only
# else
  if (pfm != WS_NS::PlatformWL)
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

  VkXcbSurfaceCreateInfoKHR info{
    VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
    nullptr,
    0,
    WS_NS::connectionXCB(),
    WS_NS::windowXCB(window_)
  };
  res = vkCreateXcbSurfaceKHR(inst, &info, nullptr, &surface_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create XCB surface");

// CAMetalLayer
#elif defined(VK_USE_PLATFORM_METAL_EXT)
  if (pfm != WS_NS::PlatformMAC)
    throw runtime_error("Platform mismatch");

  // TODO
  throw runtime_error("Unimplemented");

#else
# error "Invalid platform"
#endif // defined(VK_USE_PLATFORM_WAYLAND_KHR)

  // Check presentation support
  auto physDev = DeviceVK::get().physicalDev();
  VkBool32 supported;
  res = vkGetPhysicalDeviceSurfaceSupportKHR(physDev, family_, surface_,
                                             &supported);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not check presentation support for surface");
  if (!supported)
    throw UnsupportedExcept("Surface does not support presentation");
}

void WsiVK::initSwapchain() {
  assert(surface_ != VK_NULL_HANDLE);
  assert(swapchain_ == VK_NULL_HANDLE);

  auto physDev = DeviceVK::get().physicalDev();
  VkResult res;

  // Get surface capabilities
  VkSurfaceCapabilitiesKHR capab;
  res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDev, surface_, &capab);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not query surface capabilities");

  // Get surface formats
  vector<VkSurfaceFormatKHR> fmts;
  uint32_t fmtN;
  res = vkGetPhysicalDeviceSurfaceFormatsKHR(physDev, surface_, &fmtN, nullptr);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not query surface formats");
  fmts.resize(fmtN);
  res = vkGetPhysicalDeviceSurfaceFormatsKHR(physDev, surface_, &fmtN,
                                             fmts.data());
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not query surface formats");

  // Choose a suitable format for the swapchain
  array<VkFormat, 2> prefFmts{VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_R8G8B8A8_SRGB};

  auto fmtIt = find_first_of(fmts.begin(), fmts.end(),
                             prefFmts.begin(), prefFmts.end(),
                             [](const auto& fmt, const auto& pref) {
                               return fmt.format == pref &&
                                      fmt.colorSpace ==
                                      VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; });

  if (fmtIt == fmts.end()) {
    fmtIt = find_if(fmts.begin(), fmts.end(),
                    [](const auto& fmt) { return fromFormatVK(fmt.format) !=
                                                 PxFormatUndefined; });
    if (fmtIt == fmts.end())
      throw UnsupportedExcept("No supported format found for swapchain");
  }

  // Get surface presentation modes
  vector<VkPresentModeKHR> presModes;
  uint32_t presModeN;
  res = vkGetPhysicalDeviceSurfacePresentModesKHR(physDev, surface_, &presModeN,
                                                  nullptr);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not query surface present modes");
  presModes.resize(presModeN);
  res = vkGetPhysicalDeviceSurfacePresentModesKHR(physDev, surface_, &presModeN,
                                                  presModes.data());
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not query surface present modes");

  // TODO...
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
