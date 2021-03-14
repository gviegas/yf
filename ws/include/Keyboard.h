//
// WS
// Keyboard.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_WS_KEYBOARD_H
#define YF_WS_KEYBOARD_H

#include <functional>
#include <cstdint>

#include "yf/ws/Defs.h"

WS_NS_BEGIN

/// Key codes.
///
enum KeyCode {
  KeyCodeUnknown = 0,
  KeyCodeGrave,
  KeyCode1,
  KeyCode2,
  KeyCode3,
  KeyCode4,
  KeyCode5,
  KeyCode6,
  KeyCode7,
  KeyCode8,
  KeyCode9,
  KeyCode0,
  KeyCodeMinus,
  KeyCodeEqual,
  KeyCodeBackspace,
  KeyCodeTab,
  KeyCodeQ,
  KeyCodeW,
  KeyCodeE,
  KeyCodeR,
  KeyCodeT,
  KeyCodeY,
  KeyCodeU,
  KeyCodeI,
  KeyCodeO,
  KeyCodeP,
  KeyCodeLBracket,
  KeyCodeRBracket,
  KeyCodeBackslash,
  KeyCodeCapsLock,
  KeyCodeA,
  KeyCodeS,
  KeyCodeD,
  KeyCodeF,
  KeyCodeG,
  KeyCodeH,
  KeyCodeJ,
  KeyCodeK,
  KeyCodeL,
  KeyCodeSemicolon,
  KeyCodeApostrophe,
  KeyCodeReturn,
  KeyCodeLShift,
  KeyCodeZ,
  KeyCodeX,
  KeyCodeC,
  KeyCodeV,
  KeyCodeB,
  KeyCodeN,
  KeyCodeM,
  KeyCodeComma,
  KeyCodeDot,
  KeyCodeSlash,
  KeyCodeRShift,
  KeyCodeLCtrl,
  KeyCodeLAlt,
  KeyCodeLMeta,
  KeyCodeSpace,
  KeyCodeRMeta,
  KeyCodeRAlt,
  KeyCodeRCtrl,
  KeyCodeEsc,
  KeyCodeF1,
  KeyCodeF2,
  KeyCodeF3,
  KeyCodeF4,
  KeyCodeF5,
  KeyCodeF6,
  KeyCodeF7,
  KeyCodeF8,
  KeyCodeF9,
  KeyCodeF10,
  KeyCodeF11,
  KeyCodeF12,
  KeyCodeInsert,
  KeyCodeDelete,
  KeyCodeHome,
  KeyCodeEnd,
  KeyCodePageUp,
  KeyCodePageDown,
  KeyCodeUp,
  KeyCodeDown,
  KeyCodeLeft,
  KeyCodeRight,
  KeyCodeSysrq,
  KeyCodeScrollLock,
  KeyCodePause,
  KeyCodePadNumLock,
  KeyCodePadSlash,
  KeyCodePadStar,
  KeyCodePadMinus,
  KeyCodePadPlus,
  KeyCodePad1,
  KeyCodePad2,
  KeyCodePad3,
  KeyCodePad4,
  KeyCodePad5,
  KeyCodePad6,
  KeyCodePad7,
  KeyCodePad8,
  KeyCodePad9,
  KeyCodePad0,
  KeyCodePadDot,
  KeyCodePadEnter,
  KeyCodePadEqual,
  KeyCodeF13,
  KeyCodeF14,
  KeyCodeF15,
  KeyCodeF16,
  KeyCodeF17,
  KeyCodeF18,
  KeyCodeF19,
  KeyCodeF20,
  KeyCodeF21,
  KeyCodeF22,
  KeyCodeF23,
  KeyCodeF24
};

/// Key modifiers.
///
enum KeyMod : uint32_t {
  KeyModCapsLock = 0x01,
  KeyModShift    = 0x02,
  KeyModCtrl     = 0x04,
  KeyModAlt      = 0x08
};
using KeyModMask = uint32_t;

/// Key states.
///
enum KeyState {
  KeyStateReleased = 0,
  KeyStatePressed  = 1
};

class Window;

/// Keyboard event delegate.
///
struct KbDelegate {
  /// Enter window event (focus gained).
  ///
  std::function<void (Window*)> enter;

  /// Leave window event (focus lost).
  ///
  std::function<void (Window*)> leave;

  /// Key press/release event.
  ///
  std::function<void (KeyCode, KeyState, KeyModMask)> key;
};

/// Sets the keyboard event delegate.
///
void setDelegate(const KbDelegate& delegate);

WS_NS_END

#endif // YF_WS_KEYBOARD_H
