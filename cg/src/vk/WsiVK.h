//
// CG
// WsiVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_WSIVK_H
#define YF_CG_WSIVK_H

#include <unordered_map>
#include <unordered_set>

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
  uint32_t minImgN_ = 0;
  std::vector<Image*> images_{};
  std::unordered_map<Image*, uint32_t> indices_{};
  std::unordered_set<uint32_t> acquisitions_{};
  uint32_t acqLimit_ = 0;
  std::vector<VkSemaphore> acqSemaphores_{};
};

CG_NS_END

#endif // YF_CG_WSIVK_H
