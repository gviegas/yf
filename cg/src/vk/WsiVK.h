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

 private:
  VkSurfaceKHR surface_ = VK_NULL_HANDLE;
  VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
  std::vector<Image*> images_{};
  int32_t next_ = -1;
};

CG_NS_END

#endif // YF_CG_WSIVK_H
