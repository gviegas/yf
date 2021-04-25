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

/// Keyboard enter window event (focus gained).
///
using KbEnterFn = std::function<void (Window*)>;
void onKbEnter(KbEnterFn fn);

/// Keyboard leave window event (focus lost).
///
using KbLeaveFn = std::function<void (Window*)>;
void onKbLeave(KbLeaveFn fn);

/// Keyboard key press/release event.
///
using KbKeyFn = std::function<void (KeyCode, KeyState, KeyModMask)>;
void onKbKey(KbKeyFn fn);

WS_NS_END

#endif // YF_WS_KEYBOARD_H
