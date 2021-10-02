//
// WS
// EventXCB.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cstdlib>

#include "EventXCB.h"
#include "WindowXCB.h"
#include "KeymapUNIX.h"
#include "Delegate.h"

using namespace WS_NS;

void WS_NS::dispatchXCB() {
  const auto& vars = varsXCB();
  xcb_generic_event_t* event = nullptr;
  uint32_t type;

  // Handle KEY_PRESS/KEY_RELEASE
  auto key = [&] {
    auto ev = reinterpret_cast<xcb_key_press_event_t*>(event);

    KeyCode code = toKeyCodeUNIX(ev->detail - 8);

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

    delegate().kbKey_(code, state, modMask);
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
      // TODO: Scroll
    default:
      btn = ButtonUnknown;
    }

    ButtonState state;
    if (type == XCB_BUTTON_PRESS)
      state = ButtonStatePressed;
    else
      state = ButtonStateReleased;

    delegate().ptButton_(btn, state, ev->event_x, ev->event_y);
  };

  // Handle MOTION_NOTIFY
  auto motion = [&] {
    auto ev = reinterpret_cast<xcb_motion_notify_event_t*>(event);
    delegate().ptMotion_(ev->event_x, ev->event_y);
  };

  // Handle ENTER_NOTIFY
  auto enter = [&] {
    auto ev = reinterpret_cast<xcb_enter_notify_event_t*>(event);
    delegate().ptEnter_(WindowXCB::fromId(ev->event), ev->event_x, ev->event_y);
  };

  // Handle LEAVE_NOTIFY
  auto leave = [&] {
    auto ev = reinterpret_cast<xcb_leave_notify_event_t*>(event);
    delegate().ptLeave_(WindowXCB::fromId(ev->event));
  };

  // Handle FOCUS_IN
  auto focusIn = [&] {
    auto ev = reinterpret_cast<xcb_focus_in_event_t*>(event);
    delegate().kbEnter_(WindowXCB::fromId(ev->event));
  };

  // Handle FOCUS_OUT
  auto focusOut = [&] {
    auto ev = reinterpret_cast<xcb_focus_out_event_t*>(event);
    delegate().kbLeave_(WindowXCB::fromId(ev->event));
  };

  // Handle EXPOSE
  auto expose = [&] {
    // TODO
  };

  // Handle CONFIGURE_NOTIFY
  auto config = [&] {
    auto ev = reinterpret_cast<xcb_configure_notify_event_t*>(event);
    delegate().wdResize_(WindowXCB::fromId(ev->event), ev->width, ev->height);
    // TODO: Notify window object
  };

  // Handle CLIENT_MESSAGE
  auto client = [&] {
    auto ev = reinterpret_cast<xcb_client_message_event_t*>(event);

    if (ev->type == vars.protocolAtom && ev->data.data32[0] == vars.deleteAtom)
      delegate().wdClose_(WindowXCB::fromId(ev->window));
  };

  // Poll events
  do {
    event = pollForEventXCB(vars.connection);
    if (!event)
      break;
    type = event->response_type & ~0x80;

    switch (type) {
    case XCB_KEY_PRESS:
    case XCB_KEY_RELEASE:
      if (delegate().mask_ & Delegate::KbKey)
        key();
      break;

    case XCB_BUTTON_PRESS:
    case XCB_BUTTON_RELEASE:
      if (delegate().mask_ & Delegate::PtButton)
        button();
      break;

    case XCB_MOTION_NOTIFY:
      if (delegate().mask_ & Delegate::PtMotion)
        motion();
      break;

    case XCB_ENTER_NOTIFY:
      if (delegate().mask_ & Delegate::PtEnter)
        enter();
      break;

    case XCB_LEAVE_NOTIFY:
      if (delegate().mask_ & Delegate::PtLeave)
        leave();
      break;

    case XCB_FOCUS_IN:
      if (delegate().mask_ & Delegate::KbEnter)
        focusIn();
      break;

    case XCB_FOCUS_OUT:
      if (delegate().mask_ & Delegate::KbLeave)
        focusOut();
      break;

    case XCB_EXPOSE:
      expose();
      break;

    case XCB_CONFIGURE_NOTIFY:
      if (delegate().mask_ & Delegate::WdResize)
        config();
      break;

    case XCB_CLIENT_MESSAGE:
      if (delegate().mask_ & Delegate::WdClose)
        client();
      break;
    }

    free(event);
  } while (true);
}
