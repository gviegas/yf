//
// CG
// WsiVK.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_WSIVK_H
#define YF_CG_WSIVK_H

#include <unordered_set>

#include "Wsi.h"
#include "VK.h"

CG_NS_BEGIN

class WsiVK final : public Wsi {
 public:
  WsiVK(WS_NS::Window* window);
  ~WsiVK();

  Image* operator[](Index index);
  const Image* operator[](Index index) const;
  Image* const* begin();
  const Image* const* begin() const;
  Image* const* end();
  const Image* const* end() const;
  uint32_t size() const;
  uint32_t maxImages() const;
  std::pair<Image*, Index> nextImage(bool nonblocking);
  void present(Index imageIndex);
  WS_NS::Window& window();

  /// Checks whether a given physical device supports presentation.
  ///
  static bool checkPhysicalDevice(VkPhysicalDevice device, int32_t family);

  /// Sets the presentation queue.
  ///
  static void setQueue(VkQueue queue, int32_t family);

 private:
  static VkQueue queue_;
  static int32_t family_;

  WS_NS::Window* window_{};

  VkSurfaceKHR surface_ = VK_NULL_HANDLE;
  VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
  VkSwapchainCreateInfoKHR scInfo_{};
  uint32_t minImgN_ = 0;

  std::vector<Image*> images_{};
  std::unordered_set<uint32_t> acquisitions_{};
  uint32_t acqLimit_ = 0;
  std::vector<VkSemaphore> acqSemaphores_{};

  void initSurface();
  void querySurface();
  void createSwapchain();
};

CG_NS_END

#endif // YF_CG_WSIVK_H
