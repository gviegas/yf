//
// WS
// WindowXCB.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_WS_WINDOWXCB_H
#define YF_WS_WINDOWXCB_H

#include "Window.h"
#include "XCB.h"

WS_NS_BEGIN

class WindowXCB final : public Window {
 public:
  WindowXCB(uint32_t, uint32_t, const std::wstring&, CreationMask);
  ~WindowXCB();
  void open();
  void close();
  void setTitle(const std::wstring&);
  void toggleFullscreen();
  void resize(uint32_t, uint32_t);
  uint32_t width() const;
  uint32_t height() const;
  const std::wstring& title() const;

  /// Getter.
  ///
  xcb_window_t window() const;

  /// Gets the window object corresponding to a given window id.
  ///
  static WindowXCB* fromId(xcb_window_t);

 private:
  xcb_window_t window_ = 0;
  uint32_t width_ = 0;
  uint32_t height_ = 0;
  std::wstring title_{};
  CreationMask mask_ = 0;
  bool fullscreen_ = false;
  bool mapped_ = false;
};

WS_NS_END

#endif // YF_WS_WINDOWXCB_H
