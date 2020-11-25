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
#include "QueueVK.h"
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
  querySurface();
  createSwapchain();
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

void WsiVK::querySurface() {
  assert(surface_ != VK_NULL_HANDLE);

  auto physDev = DeviceVK::get().physicalDev();
  VkResult res;

  // Get surface capabilities
  VkSurfaceCapabilitiesKHR capab;
  res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDev, surface_, &capab);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not query surface capabilities");

  // Choose a suitable image count
  uint32_t imgCount = capab.minImageCount;
  if (capab.maxImageCount == 0)
    // Triple buffered or more
    imgCount = max(capab.minImageCount, 3U);

  // Choose a suitable composite alpha
  VkCompositeAlphaFlagBitsKHR compAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
  if (!(compAlpha & capab.supportedCompositeAlpha)) {
    compAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    if (!(compAlpha & capab.supportedCompositeAlpha))
      throw UnsupportedExcept("No suitable alpha compositing mode available");
  }

  // Ensure correct dimensions
  VkExtent2D extent{window_->width(), window_->height()};
  if (capab.currentExtent.width == 0xFFFFFFFF) {
    extent.width = clamp(extent.width, capab.minImageExtent.width,
                         capab.maxImageExtent.width);

    extent.height = clamp(extent.height, capab.minImageExtent.height,
                          capab.maxImageExtent.height);
  }

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

  // Choose a suitable format
  array<VkFormat, 2> prefFmts{VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM};

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
      throw UnsupportedExcept("Surface format(s) not supported");
  }

//  // Get surface presentation modes
//  vector<VkPresentModeKHR> presModes;
//  uint32_t presModeN;
//  res = vkGetPhysicalDeviceSurfacePresentModesKHR(physDev, surface_, &presModeN,
//                                                  nullptr);
//  if (res != VK_SUCCESS)
//    throw DeviceExcept("Could not query surface present modes");
//  presModes.resize(presModeN);
//  res = vkGetPhysicalDeviceSurfacePresentModesKHR(physDev, surface_, &presModeN,
//                                                  presModes.data());
//  if (res != VK_SUCCESS)
//    throw DeviceExcept("Could not query surface present modes");

  // No compelling reason to use a different present mode currently
  VkPresentModeKHR presMode = VK_PRESENT_MODE_FIFO_KHR;

  // Define sharing mode
  uint32_t families[2];
  uint32_t familyN = 0;
  VkSharingMode sharMode = VK_SHARING_MODE_EXCLUSIVE;
  auto& queue = static_cast<QueueVK&>(DeviceVK::get().defaultQueue());
  if (family_ != queue.family()) {
    families[0] = queue.family();
    families[1] = family_;
    sharMode = VK_SHARING_MODE_CONCURRENT;
  }

  // Set swapchain create info
  scInfo_.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  scInfo_.pNext = nullptr;
  scInfo_.flags = 0;
  scInfo_.surface = surface_;
  scInfo_.minImageCount = imgCount;
  scInfo_.imageFormat = fmtIt->format;
  scInfo_.imageColorSpace = fmtIt->colorSpace;
  scInfo_.imageExtent = extent;
  scInfo_.imageArrayLayers = 1;
  scInfo_.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  scInfo_.imageSharingMode = sharMode;
  scInfo_.queueFamilyIndexCount = familyN;
  scInfo_.pQueueFamilyIndices = families;
  scInfo_.preTransform = capab.currentTransform;
  scInfo_.compositeAlpha = compAlpha;
  scInfo_.presentMode = presMode;
  scInfo_.clipped = true;
  scInfo_.oldSwapchain = VK_NULL_HANDLE;
}

void WsiVK::createSwapchain() {
  assert(surface_ != VK_NULL_HANDLE);

  auto dev = DeviceVK::get().device();
  VkResult res;

  // Create swapchain
  if (swapchain_ != VK_NULL_HANDLE) {
    scInfo_.oldSwapchain = swapchain_;
    res = vkCreateSwapchainKHR(dev, &scInfo_, nullptr, &swapchain_);
    vkDestroySwapchainKHR(dev, scInfo_.oldSwapchain, nullptr);
  } else {
    res = vkCreateSwapchainKHR(dev, &scInfo_, nullptr, &swapchain_);
  }
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create swapchain");

  // Get image handles
  vector<VkImage> imgHandles;
  uint32_t imgHandleN;
  res = vkGetSwapchainImagesKHR(dev, swapchain_, &imgHandleN, nullptr);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not get swapchain images");
  imgHandles.resize(imgHandleN);
  res = vkGetSwapchainImagesKHR(dev, swapchain_, &imgHandleN,
                                imgHandles.data());
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not get swapchain images");

  // Wrap image handles
  for_each(images_.begin(), images_.end(), [](auto img) { delete img; });
  images_.clear();
  auto fmt = fromFormatVK(scInfo_.imageFormat);
  Size2 sz{scInfo_.imageExtent.width, scInfo_.imageExtent.height};
  for (auto& ih : imgHandles)
    images_.push_back(new ImageVK(fmt, sz, 1, 1, Samples1,
                                  VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL,
                                  scInfo_.imageUsage, ih, nullptr,
                                  VK_IMAGE_LAYOUT_UNDEFINED, false));

  // Create image acquisition semaphore
  if (nextSem_ == VK_NULL_HANDLE) {
    VkSemaphoreCreateInfo semInfo;
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semInfo.pNext = nullptr;
    semInfo.flags = 0;

    res = vkCreateSemaphore(dev, &semInfo, nullptr, &nextSem_);
    if (res != VK_SUCCESS)
      throw DeviceExcept("Could not create image acquisition semaphore");
  }
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
