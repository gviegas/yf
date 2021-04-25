//
// WS
// Delegate.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_WS_DELEGATE_H
#define YF_WS_DELEGATE_H

#include "Event.h"
#include "Window.h"
#include "Keyboard.h"
#include "Pointer.h"

WS_NS_BEGIN

/// Delegation manager.
///
class Delegate {
 public:
  /// Mask of `Flags` bits.
  ///
  using Mask = uint32_t;

  /// Flags representing events.
  ///
  enum Flags : uint32_t {
    WdClose  = 0x001,
    WdResize = 0x002,
    KbEnter  = 0x004,
    KbLeave  = 0x008,
    KbKey    = 0x010,
    PtEnter  = 0x020,
    PtLeave  = 0x040,
    PtMotion = 0x080,
    PtButton = 0x100
  };

  /// Variables for delegation management.
  ///
  /// The WS `on*()` functions update the state of these variables as needed.
  /// Implementations read from these variables to identify which events
  /// must be dispatched.
  ///
  Mask mask_ = 0;
  WdCloseFn wdClose_{};
  WdResizeFn wdResize_{};
  KbEnterFn kbEnter_{};
  KbLeaveFn kbLeave_{};
  KbKeyFn kbKey_{};
  PtEnterFn ptEnter_{};
  PtLeaveFn ptLeave_{};
  PtMotionFn ptMotion_{};
  PtButtonFn ptButton_{};
};

/// Gets the global delegate instance.
///
const Delegate& delegate();

WS_NS_END

#endif // YF_WS_DELEGATE_H
