//
// WS
// Window.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_WS_WINDOW_H
#define YF_WS_WINDOW_H

#include <cstdint>
#include <string>
#include <memory>
#include <functional>

#include "yf/ws/Defs.h"

WS_NS_BEGIN

/// Window.
///
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

  /// Maximum length of app ID, in bytes.
  ///
  static constexpr uint32_t AppIdLen = 64;

  /// Maximum length of window title, in bytes.
  ///
  static constexpr uint32_t TitleLen = 80;

  Window() = default;
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  virtual ~Window();

  /// The app ID that all windows will be associated with.
  ///
  static std::wstring appId;

  /// Makes a new window object.
  ///
  static Ptr make(uint32_t width,
                  uint32_t height,
                  const std::wstring& title,
                  CreationMask mask = Resizable);

  /// Opens the window.
  ///
  virtual void open() = 0;

  /// Closes the window.
  ///
  virtual void close() = 0;

  /// Sets the window title.
  ///
  virtual void setTitle(const std::wstring& title) = 0;

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
  virtual const std::wstring& title() const = 0;
};

/// Window event delegate.
///
struct WdDelegate {
  /// Close event.
  ///
  std::function<void (Window*)> close;

  /// Resize event.
  ///
  std::function<void (Window*, uint32_t w, uint32_t h)> resize;
};

WS_NS_END

#endif // YF_WS_WINDOW_H
