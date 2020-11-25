//
// CG
// WsiVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_WSIVK_H
#define YF_CG_WSIVK_H

#include "Wsi.h"
#include "VK.h"

CG_NS_BEGIN

class WsiVK final : Wsi {
 public:
  WsiVK(WS_NS::Window* window);
  ~WsiVK();
  const std::vector<Image*>& images() const;
  Image* nextImage();
  void present();

  /// Checks whether a given physical device supports presentation.
  ///
  static bool checkPhysicalDevice(VkPhysicalDevice device, int32_t family);

  /// Sets the presentation queue.
  ///
  static void setQueue(VkQueue queue, int32_t family);

 private:
  void initSurface();
  void querySurface();
  void createSwapchain();

  static VkQueue queue_;
  static int32_t family_;
  VkSurfaceKHR surface_ = VK_NULL_HANDLE;
  VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
  VkSwapchainCreateInfoKHR scInfo_{};
  std::vector<Image*> images_{};
  int32_t next_ = -1;
  VkSemaphore nextSem_ = VK_NULL_HANDLE;
};

CG_NS_END

#endif // YF_CG_WSIVK_H
