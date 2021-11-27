//
// CG
// Wsi.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_WSI_H
#define YF_CG_WSI_H

#include <cstdint>
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

  Wsi() = default;
  Wsi(const Wsi&) = delete;
  Wsi& operator=(const Wsi&) = delete;
  virtual ~Wsi() = default;

  /// Gets the swapchain image for a givem index.
  ///
  virtual Image* operator[](Index index) = 0;
  virtual const Image* operator[](Index index) const = 0;

  /// Gets the beginning of the swapchain.
  ///
  virtual Image* const* begin() = 0;
  virtual const Image* const* begin() const = 0;

  /// Gets the end of the swapchain.
  ///
  virtual Image* const* end() = 0;
  virtual const Image* const* end() const = 0;

  /// Gets the number of images in the swapchain.
  ///
  virtual uint32_t size() const = 0;

  /// Gets the maximum number of images that can be acquired.
  ///
  virtual uint32_t acquisitionLimit() const = 0;

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
