//
// WS
// Pointer.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_WS_POINTER_H
#define YF_WS_POINTER_H

#include <utility>

#include "yf/ws/Defs.h"

WS_NS_BEGIN

/// The current (x, y) coordinates of the pointer.
///
std::pair<float, float> pointerLocation();

/// Pointer buttons.
///
enum Button {
  ButtonUnknown,
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

WS_NS_END

#endif // YF_WS_POINTER_H
