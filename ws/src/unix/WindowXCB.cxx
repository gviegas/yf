//
// WS
// WindowXCB.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cstdlib>

#include "WindowXCB.h"
#include "yf/Except.h"

using namespace WS_NS;
using namespace std;

WindowXCB::WindowXCB(uint32_t width, uint32_t height, CreationMask mask)
  : width_(width), height_(height), mask_(mask) {

  const auto& vars = varsXCB();

  window_ = generateIdXCB(vars.connection);

  xcb_void_cookie_t cookie;
  xcb_generic_error_t* err = nullptr;

  auto deinit = [&] {
    free(err);
    if (window_)
      destroyWindowXCB(vars.connection, window_);
  };

  uint32_t valMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  uint32_t valList[2];
  valList[0] = vars.blackPixel;
  valList[1] = XCB_EVENT_MASK_KEY_PRESS |
               XCB_EVENT_MASK_KEY_RELEASE |
               XCB_EVENT_MASK_BUTTON_PRESS |
               XCB_EVENT_MASK_BUTTON_RELEASE |
               XCB_EVENT_MASK_ENTER_WINDOW |
               XCB_EVENT_MASK_LEAVE_WINDOW |
               XCB_EVENT_MASK_POINTER_MOTION |
               XCB_EVENT_MASK_BUTTON_MOTION |
               XCB_EVENT_MASK_EXPOSURE |
               XCB_EVENT_MASK_STRUCTURE_NOTIFY |
               XCB_EVENT_MASK_FOCUS_CHANGE;

  cookie = createWindowCheckedXCB(vars.connection, 0, window_, vars.root,
                                  0, 0, width, height, 0,
                                  XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                  vars.visualId, valMask, valList);

  err = requestCheckXCB(vars.connection, cookie);
  if (err) {
    deinit();
    throw runtime_error("createWindowCheckedXCB failed");
  }

  // TODO: change properties

  // TODO: check mask

  if (flushXCB(vars.connection) <= 0) {
    deinit();
    throw runtime_error("flushXCB failed");
  }
}

WindowXCB::~WindowXCB() {
  // TODO
}

void WindowXCB::close() {
  // TODO
}

void WindowXCB::toggleFullscreen() {
  // TODO
}

void WindowXCB::resize(uint32_t width, uint32_t height) {
  // TODO
}

void WindowXCB::show() {
  // TODO
}

void WindowXCB::hide() {
  // TODO
}

uint32_t WindowXCB::width() const {
  return width_;
}

uint32_t WindowXCB::height() const {
  return height_;
}
