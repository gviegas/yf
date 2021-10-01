//
// CG
// WsiVK.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <algorithm>
#include <cassert>

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

WsiVK::WsiVK(WS_NS::Window& window) : window_(window) {
  if (!queue_ || family_ < 0)
    throw UnsupportedExcept("Wsi not supported");

  initSurface();
  querySurface();
  createSwapchain();
}

WsiVK::~WsiVK() {
  // Flush and present acquired images
  if (!acquisitions_.empty()) {
    deviceVK().defaultQueue().submit();

    for_each(acquisitions_.begin(), acquisitions_.end(), [&](auto& i) {
      present(i);
    });
  }

  // Wait until finished
  vkQueueWaitIdle(queue_);

  // Destroy objects
  auto dev = deviceVK().device();
  auto inst = deviceVK().instance();
  vkDestroySwapchainKHR(dev, swapchain_, nullptr);
  vkDestroySurfaceKHR(inst, surface_, nullptr);
  for_each(images_.begin(), images_.end(), [](auto& img) { delete img; });
  for_each(acqSemaphores_.begin(), acqSemaphores_.end(), [&](auto& sem) {
    vkDestroySemaphore(dev, sem, nullptr);
  });
}

void WsiVK::initSurface() {
  assert(surface_ == VK_NULL_HANDLE);

  auto pfm = WS_NS::platform();
  auto inst = deviceVK().instance();
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
      WS_NS::windowXCB(&window_)
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
  auto physDev = deviceVK().physicalDev();
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

  auto physDev = deviceVK().physicalDev();
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
  VkExtent2D extent{window_.width(), window_.height()};
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
  constexpr VkFormat prefFmt = VK_FORMAT_B8G8R8A8_SRGB;

  auto fmtIt = find_if(fmts.begin(), fmts.end(), [&](const auto& fmt) {
    return fmt.format == prefFmt &&
           fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  });

  if (fmtIt == fmts.end()) {
    fmtIt = find_if(fmts.begin(), fmts.end(), [](const auto& fmt) {
      return fromFormatVK(fmt.format) != PxFormatUndefined;
    });
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
  auto& queue = static_cast<QueueVK&>(deviceVK().defaultQueue());
  if (family_ != queue.family()) {
    families[0] = queue.family();
    families[1] = family_;
    familyN = 2;
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

  // [1.2.166 c33.9]
  // "An image will eventually be acquired if the number of images that the
  // application has currently acquired (but not yet presented) is less than
  // or equal to the difference between the number of images in swapchain
  // and the value of VkSurfaceCapabilitiesKHR::minImageCount. If the number
  // of currently acquired images is greater than this, vkAcquireNextImageKHR
  // should not be called; if it is, timeout must not be UINT64_MAX."

  minImgN_ = capab.minImageCount;
}

void WsiVK::createSwapchain() {
  assert(surface_ != VK_NULL_HANDLE);

  auto dev = deviceVK().device();
  VkResult res;

  // Create swapchain
  if (swapchain_ != VK_NULL_HANDLE) {
    scInfo_.oldSwapchain = swapchain_;
    swapchain_ = VK_NULL_HANDLE;
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
  for_each(images_.begin(), images_.end(), [](auto& img) { delete img; });
  images_.clear();
  auto fmt = fromFormatVK(scInfo_.imageFormat);
  Size2 sz{scInfo_.imageExtent.width, scInfo_.imageExtent.height};
  for (auto& ih : imgHandles)
    images_.push_back(new ImageVK(fmt, sz, 1, 1, Samples1,
                                  VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL,
                                  scInfo_.imageUsage, ih, nullptr,
                                  VK_IMAGE_LAYOUT_UNDEFINED, false));

  // Clear image acquisitions & set new limit
  acquisitions_.clear();
  acqLimit_ = 1 + images_.size() - minImgN_;

  // Create image acquisition semaphores
  for_each(acqSemaphores_.begin(), acqSemaphores_.end(), [&](auto& sem) {
    vkDestroySemaphore(dev, sem, nullptr);
  });
  acqSemaphores_.clear();

  VkSemaphoreCreateInfo semInfo;
  semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semInfo.pNext = nullptr;
  semInfo.flags = 0;

  VkSemaphore sem;
  auto semN = images_.size();
  do {
    res = vkCreateSemaphore(dev, &semInfo, nullptr, &sem);
    if (res != VK_SUCCESS)
      throw DeviceExcept("Could not create image acquisition semaphore");
    acqSemaphores_.push_back(sem);
  } while (--semN);
}

Image* WsiVK::operator[](Index index) {
  assert(index < images_.size());
  return images_[index];
}

const Image* WsiVK::operator[](Index index) const {
  assert(index < images_.size());
  return images_[index];
}

Image* const* WsiVK::begin() {
  return images_.data();
}

const Image* const* WsiVK::begin() const {
  return images_.data();
}

Image* const* WsiVK::end() {
  return images_.data()+images_.size();
}

const Image* const* WsiVK::end() const {
  return images_.data()+images_.size();
}

uint32_t WsiVK::size() const {
  return images_.size();
}

uint32_t WsiVK::acquisitionLimit() const {
  return acqLimit_;
}

pair<Image*, Wsi::Index> WsiVK::nextImage(bool nonblocking) {
  if (acquisitions_.size() == acqLimit_)
    throw LimitExcept("Limit for Wsi image acquisitions reached");

  const uint64_t timeout = nonblocking ? 0 : UINT64_MAX;

  VkSemaphore sem;
  uint32_t semIx = 0;
  for (;; semIx++) {
    if (acquisitions_.find(semIx) == acquisitions_.end()) {
      sem = acqSemaphores_[semIx];
      break;
    }
  }
  uint32_t imgIx;
  auto dev = deviceVK().device();
  auto res = vkAcquireNextImageKHR(dev, swapchain_, timeout,
                                   sem, VK_NULL_HANDLE, &imgIx);

  auto& que = static_cast<QueueVK&>(deviceVK().defaultQueue());

  switch (res) {
  case VK_SUCCESS:
    if (semIx != imgIx)
      swap(acqSemaphores_[semIx], acqSemaphores_[imgIx]);
    que.waitFor(sem, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    acquisitions_.insert(imgIx);
    return {images_[imgIx], imgIx};

  case VK_TIMEOUT:
  case VK_NOT_READY:
    return {nullptr, UINT32_MAX};

  case VK_SUBOPTIMAL_KHR:
  case VK_ERROR_OUT_OF_DATE_KHR:
    // TODO: notify and recreate swapchain
    throw runtime_error("Broken swapchain handling not implemented");

  case VK_ERROR_SURFACE_LOST_KHR:
    // TODO: notify and (try to) recreate surface and swapchain
    throw runtime_error("Lost surface handling not implemented");

//  case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:

  default:
    throw DeviceExcept("Could not acquire swapchain image");
  }
}

void WsiVK::present(Index imageIndex) {
  if (imageIndex >= images_.size())
    throw invalid_argument("Invalid image index for presentation");
  if (acquisitions_.erase(imageIndex) == 0)
    throw invalid_argument("Cannot present an image that was not acquired");

  auto img = static_cast<ImageVK*>(images_[imageIndex]);
  auto imgLay = img->layout();

  if (imgLay.first != imgLay.second)
    throw runtime_error("Cannot present an image pending transition");

  // Change layout for presentation (expects general layout as the current one)
  VkImageMemoryBarrier barrier;
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.pNext = nullptr;
  barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
  barrier.dstAccessMask = 0;
  barrier.oldLayout = imgLay.first;
  barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = img->handle();
  barrier.subresourceRange.aspectMask = aspectOfVK(img->format());
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  // XXX: the following assumes that all work has been submitted for execution
  // and there's nothing pending in the priority buffer (this will have to be
  // amended eventually)

  // Ideally, this transition should be encoded after the last use of image
  img->changeLayout(barrier, false);

  // Present
  VkPresentInfoKHR info;
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.pNext = nullptr;
  info.waitSemaphoreCount = 0;
  info.pWaitSemaphores = nullptr;
  info.swapchainCount = 1;
  info.pSwapchains = &swapchain_;
  info.pImageIndices = &imageIndex;
  info.pResults = nullptr;

  auto res = vkQueuePresentKHR(queue_, &info);

  // XXX: may want to change layout back to general

  switch (res) {
  case VK_SUCCESS:
    return;

  case VK_SUBOPTIMAL_KHR:
  case VK_ERROR_OUT_OF_DATE_KHR:
    // TODO: notify and recreate swapchain
    throw runtime_error("Broken swapchain handling not implemented");

  case VK_ERROR_SURFACE_LOST_KHR:
    // TODO: notify and (try to) recreate surface and swapchain
    throw runtime_error("Lost surface handling not implemented");

//  case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:

  default:
    throw DeviceExcept("Could not present image");
  }
}

WS_NS::Window& WsiVK::window() {
  return window_;
}
