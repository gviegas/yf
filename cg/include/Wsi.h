//
// CG
// Wsi.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_WSI_H
#define YF_CG_WSI_H

#include <utility>
#include <memory>
#include <vector>

#include "yf/ws/Window.h"
#include "yf/cg/Image.h"

CG_NS_BEGIN

/// Presentable surface.
///
class Wsi {
 public:
  using Ptr = std::unique_ptr<Wsi>;
  using Index = uint32_t;

  virtual ~Wsi();

  /// Gets the list of all images in the swapchain.
  ///
  virtual const std::vector<Image*>& images() const = 0;

  /// Gets the maximum number of images that can be acquired.
  ///
  virtual uint32_t maxImages() const = 0;

  /// Gets the next writable image.
  ///
  virtual std::pair<Image*, Index> nextImage(bool nonblocking = true) = 0;

  /// Presents a previously acquired image.
  ///
  virtual void present(Index imageIndex) = 0;

  /// Gets the window object.
  ///
  virtual WS_NS::Window& window() = 0;
};

CG_NS_END

#endif // YF_CG_WSI_H
