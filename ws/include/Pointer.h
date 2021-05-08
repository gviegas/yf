//
// WS
// Pointer.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_WS_POINTER_H
#define YF_WS_POINTER_H

#include <cstdint>
#include <functional>

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
enum ButtonState {
  ButtonStateReleased = 0,
  ButtonStatePressed  = 1
};

class Window;

/// Pointer enter window event.
///
using PtEnterFn = std::function<void (Window*, int32_t x, int32_t y)>;
void onPtEnter(PtEnterFn fn);

/// Pointer leave window event.
///
using PtLeaveFn = std::function<void (Window*)>;
void onPtLeave(PtLeaveFn fn);

/// Pointer motion event.
///
using PtMotionFn = std::function<void (int32_t x, int32_t y)>;
void onPtMotion(PtMotionFn fn);

/// Pointer button press/release event.
///
using PtButtonFn = std::function<void (Button, ButtonState,
                                       int32_t x, int32_t y)>;
void onPtButton(PtButtonFn fn);

WS_NS_END

#endif // YF_WS_POINTER_H
