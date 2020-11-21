//
// WS
// Window.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_WS_WINDOW_H
#define YF_WS_WINDOW_H

#include <cstdint>
#include <memory>

#include "yf/ws/Defs.h"

WS_NS_BEGIN

class Window {
 public:
  using Ptr = std::unique_ptr<Window>;

  /// Mask of `CreationFlags` bits.
  ///
  using CreationMask = uint32_t;

  /// Window creation flags.
  ///
  enum CreationFlags : uint32_t {
    Fullscreen = 0x01,
    Borderless = 0x02,
    Resizable  = 0x04,
    Hidden     = 0x08
  };

  Window() = default;
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  virtual ~Window();

  /// Makes a new window object.
  ///
  static Ptr make(uint32_t width, uint32_t height, CreationMask mask);

  /// Opens the window.
  ///
  virtual void open() = 0;

  /// Closes the window.
  ///
  virtual void close() = 0;

  /// Toggles fullscreen mode.
  ///
  virtual void toggleFullscreen() = 0;

  /// Resizes the window.
  ///
  virtual void resize(uint32_t width, uint32_t height) = 0;

  /// Getters.
  ///
  virtual uint32_t width() const = 0;
  virtual uint32_t height() const = 0;
};

WS_NS_END

#endif // YF_WS_WINDOW_H
