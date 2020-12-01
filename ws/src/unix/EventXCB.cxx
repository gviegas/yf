//
// WS
// EventXCB.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cstdlib>

#include "EventXCB.h"

using namespace WS_NS;

EventXCB& EventXCB::get() {
  static EventXCB ev;
  return ev;
}

void EventXCB::dispatch() {
  auto conn = varsXCB().connection;
  xcb_generic_event_t* event = nullptr;
  uint32_t type;

  // Handle KEY_PRESS/KEY_RELEASE
  auto key = [&] {
    auto ev = reinterpret_cast<xcb_key_press_event_t*>(event);

    // TODO
    // = toKeyCode(event->detail - 8);
    KeyCode code = KeyCodeUnknown;

    KeyState state;
    if (type == XCB_KEY_PRESS)
      state = KeyStatePressed;
    else
      state = KeyStateReleased;

    static uint16_t prevEvState = 0;
    static KeyModMask prevModMask = 0;

    KeyModMask modMask;
    if (ev->state == prevEvState) {
      modMask = prevModMask;
    } else {
      modMask = 0;

      if (ev->state & XCB_MOD_MASK_LOCK)
        modMask |= KeyModCapsLock;
      if (ev->state & XCB_MOD_MASK_SHIFT)
        modMask |= KeyModShift;
      if (ev->state & XCB_MOD_MASK_CONTROL)
        modMask |= KeyModCtrl;
      if (ev->state & XCB_MOD_MASK_1)
        modMask |= KeyModAlt;

      prevModMask = modMask;
      prevEvState = ev->state;
    }

    kbDeleg_.key(code, state, modMask);
  };

  // Handle BUTTON_PRESS/BUTTON_RELEASE
  auto button = [&] {
    auto ev = reinterpret_cast<xcb_button_press_event_t*>(event);

    Button btn;
    switch (ev->detail) {
    case XCB_BUTTON_INDEX_1:
      btn = ButtonLeft;
      break;
    case XCB_BUTTON_INDEX_2:
      btn = ButtonMiddle;
      break;
    case XCB_BUTTON_INDEX_3:
      btn = ButtonRight;
      break;
    case XCB_BUTTON_INDEX_4:
    case XCB_BUTTON_INDEX_5:
      // TODO: scroll
    default:
      btn = ButtonUnknown;
    }

    BtnState state;
    if (type == XCB_BUTTON_PRESS)
      state = BtnStatePressed;
    else
      state = BtnStateReleased;

    ptDeleg_.button(btn, state, ev->event_x, ev->event_y);
  };

  // Handle MOTION_NOTIFY
  auto motion = [&] {
    auto ev = reinterpret_cast<xcb_motion_notify_event_t*>(event);
    // TODO...
  };

  // Handle ENTER_NOTIFY
  auto enter = [&] {
    auto ev = reinterpret_cast<xcb_enter_notify_event_t*>(event);
    // TODO...
  };

  // Handle LEAVE_NOTIFY
  auto leave = [&] {
    auto ev = reinterpret_cast<xcb_leave_notify_event_t*>(event);
    // TODO...
  };

  // Handle FOCUS_IN
  auto focusIn = [&] {
    auto ev = reinterpret_cast<xcb_focus_in_event_t*>(event);
    // TODO...
  };

  // Handle FOCUS_OUT
  auto focusOut = [&] {
    auto ev = reinterpret_cast<xcb_focus_out_event_t*>(event);
    // TODO...
  };

  // Handle EXPOSE
  auto expose = [&] {
    // TODO
  };

  // Handle CONFIGURE_NOTIFY
  auto config = [&] {
    auto ev = reinterpret_cast<xcb_configure_notify_event_t*>(event);
    // TODO...
  };

  // Handle CLIENT_MESSAGE
  auto client = [&] {
    auto ev = reinterpret_cast<xcb_client_message_event_t*>(event);
    // TODO...
  };

  // Poll events
  do {
    event = pollForEventXCB(conn);
    if (!event)
      break;
    type = event->response_type & ~0x80;

    switch (type) {
    case XCB_KEY_PRESS:
    case XCB_KEY_RELEASE:
      if (mask_ & KbKey)
        key();
      break;

    case XCB_BUTTON_PRESS:
    case XCB_BUTTON_RELEASE:
      if (mask_ & PtButton)
        button();
      break;

    case XCB_MOTION_NOTIFY:
      if (mask_ & PtMotion)
        motion();
      break;

    case XCB_ENTER_NOTIFY:
      if (mask_ & PtEnter)
        enter();
      break;

    case XCB_LEAVE_NOTIFY:
      if (mask_ & PtLeave)
        leave();
      break;

    case XCB_FOCUS_IN:
      if (mask_ & KbEnter)
        focusIn();
      break;

    case XCB_FOCUS_OUT:
      if (mask_ & KbLeave)
        focusOut();
      break;

    case XCB_EXPOSE:
      expose();
      break;

    case XCB_CONFIGURE_NOTIFY:
      if (mask_ & WdResize)
        config();
      break;

    case XCB_CLIENT_MESSAGE:
      if (mask_ & WdClose)
        client();
      break;
    }

    free(event);
  } while (true);
}

void EventXCB::setDelegate(const WdDelegate& delegate) {
  wdDeleg_ = delegate;

  mask_ = wdDeleg_.close  ? (mask_ | WdClose)  : (mask_ & ~WdClose);
  mask_ = wdDeleg_.resize ? (mask_ | WdResize) : (mask_ & ~WdResize);
}

void EventXCB::setDelegate(const KbDelegate& delegate) {
  kbDeleg_ = delegate;

  mask_ = kbDeleg_.enter ? (mask_ | KbEnter) : (mask_ & ~KbEnter);
  mask_ = kbDeleg_.leave ? (mask_ | KbLeave) : (mask_ & ~KbLeave);
  mask_ = kbDeleg_.key   ? (mask_ | KbKey)   : (mask_ & ~KbKey);
}

void EventXCB::setDelegate(const PtDelegate& delegate) {
  ptDeleg_ = delegate;

  mask_ = ptDeleg_.enter  ? (mask_ | PtEnter)  : (mask_ & ~PtEnter);
  mask_ = ptDeleg_.leave  ? (mask_ | PtLeave)  : (mask_ & ~PtLeave);
  mask_ = ptDeleg_.motion ? (mask_ | PtMotion) : (mask_ & ~PtMotion);
  mask_ = ptDeleg_.button ? (mask_ | PtButton) : (mask_ & ~PtButton);
}
