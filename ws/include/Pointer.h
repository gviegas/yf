//
// WS
// Pointer.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_WS_POINTER_H
#define YF_WS_POINTER_H

#include <utility>
#include <functional>
#include <cstdint>

#include "yf/ws/Defs.h"

WS_NS_BEGIN

/// Pointer buttons.
///
enum Button {
  ButtonUnknown = 0,
  ButtonLeft,
  ButtonRight,
  ButtonMiddle,
  ButtonSide,
  ButtonForward,
  ButtonBackward
};

/// Button states.
///
enum BtnState {
  BtnStateReleased = 0,
  BtnStatePressed  = 1
};

class Window;

/// Pointer event delegate.
///
struct PtDelegate {
  /// Enter window event.
  ///
  std::function<void (Window*, int32_t x, int32_t y)> enter;

  /// Leave window event.
  ///
  std::function<void (Window*)> leave;

  /// Motion event.
  ///
  std::function<void (int32_t x, int32_t y)> motion;

  /// Button press/release event.
  ///
  std::function<void (Button, BtnState, int32_t x, int32_t y)> button;
};

WS_NS_END

#endif // YF_WS_POINTER_H
