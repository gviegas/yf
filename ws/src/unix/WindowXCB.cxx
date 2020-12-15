//
// WS
// WindowXCB.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <unordered_map>

#include "WindowXCB.h"
#include "yf/Except.h"

using namespace WS_NS;
using namespace std;

INTERNAL_NS_BEGIN

/// Mapping between window ids and objects.
///
unordered_map<xcb_window_t, WindowXCB*> winMap{};

INTERNAL_NS_END

WindowXCB* WindowXCB::fromId(xcb_window_t id) {
  auto it = winMap.find(id);
  return it == winMap.end() ? nullptr : it->second;
}

WindowXCB::WindowXCB(uint32_t width,
                     uint32_t height,
                     const wstring& title,
                     CreationMask mask)
  : width_(width), height_(height), /*title_(title),*/ mask_(mask) {

  const auto& vars = varsXCB();
  xcb_void_cookie_t cookie;
  xcb_generic_error_t* err = nullptr;

  auto deinit = [&] {
    free(err);
    if (window_)
      destroyWindowXCB(vars.connection, window_);
  };

  // Create window
  window_ = generateIdXCB(vars.connection);

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

  // Set `delete` property (for close/quit)
  cookie = changePropertyCheckedXCB(vars.connection, XCB_PROP_MODE_REPLACE,
                                    window_, vars.protocolAtom, XCB_ATOM_ATOM,
                                    32, 1, &vars.deleteAtom);

  err = requestCheckXCB(vars.connection, cookie);
  if (err) {
    deinit();
    throw runtime_error("changePropertyCheckedXCB failed");
  }

  // Set `class` property (for app ID)
  mbstate_t state;
  memset(&state, 0, sizeof state);
  char str[AppIdLen+2] = {'\0', '\0'};
  const wchar_t* src = appId.data();
  auto len = 2 + wcsrtombs(str+1, &src, AppIdLen, &state);

  if (src)
    throw LimitExcept("Could not set window app ID");

  cookie = changePropertyCheckedXCB(vars.connection, XCB_PROP_MODE_REPLACE,
                                    window_, vars.classAtom, XCB_ATOM_STRING,
                                    8, len, str);

  err = requestCheckXCB(vars.connection, cookie);
  if (err) {
    deinit();
    throw runtime_error("changePropertyCheckedXCB failed");
  }

  // Set title
  setTitle(title);

  // Check flags
  if (mask & Fullscreen)
    // TODO
    throw runtime_error("Unimplemented");

  if (mask & Borderless)
    // TODO
    throw runtime_error("Unimplemented");

  if (!(mask & Resizable))
    // TODO
    throw runtime_error("Unimplemented");

  if (!(mask & Hidden))
    open();

  // Flush
  if (flushXCB(vars.connection) <= 0) {
    deinit();
    throw runtime_error("flushXCB failed");
  }

  // Insert mapping
  winMap.emplace(window_, this);
}

WindowXCB::~WindowXCB() {
  winMap.erase(window_);
  // XXX: this must happen before deinitXCB()
  destroyWindowXCB(varsXCB().connection, window_);
}

void WindowXCB::open() {
  if (mapped_)
    return;

  const auto& vars = varsXCB();
  auto cookie = mapWindowCheckedXCB(vars.connection, window_);
  auto err = requestCheckXCB(vars.connection, cookie);

  if (err) {
    free(err);
    throw runtime_error("mapWindowCheckedXCB failed");
  }

  mapped_ = true;
}

void WindowXCB::close() {
  if (!mapped_)
    return;

  const auto& vars = varsXCB();
  auto cookie = unmapWindowCheckedXCB(vars.connection, window_);
  auto err = requestCheckXCB(vars.connection, cookie);

  if (err) {
    free(err);
    throw runtime_error("unmapWindowCheckedXCB failed");
  }

  mapped_ = false;
}

void WindowXCB::setTitle(const std::wstring& title) {
  mbstate_t state;
  memset(&state, 0, sizeof state);
  char str[TitleLen];
  auto src = title.data();
  auto len = wcsrtombs(str, &src, TitleLen, &state);

  if (src)
    throw LimitExcept("Could not set window title");

  const auto& vars = varsXCB();

  auto cookie = changePropertyCheckedXCB(vars.connection,
                                         XCB_PROP_MODE_REPLACE,
                                         window_, vars.titleAtom,
                                         vars.utf8Atom, 8, len, str);

  auto err = requestCheckXCB(vars.connection, cookie);

  if (err) {
    free(err);
    throw runtime_error("changePropertyCheckedXCB failed");
  }

  title_ = title;
}

void WindowXCB::toggleFullscreen() {
  // TODO
}

void WindowXCB::resize(uint32_t width, uint32_t height) {
  if (!(mask_ & Resizable))
    return;

  if (width == 0 || height == 0)
    throw invalid_argument("WindowXCB resize() requires dimensions > 0");

  const auto& vars = varsXCB();
  uint32_t valMask = XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
  uint32_t valList[] = {width, height};

  auto cookie = configureWindowCheckedXCB(vars.connection, window_,
                                          valMask, valList);

  auto err = requestCheckXCB(vars.connection, cookie);

  if (err) {
    free(err);
    throw runtime_error("configureWindowCheckedXCB failed");
  }

  width_ = width;
  height_ = height;
}

uint32_t WindowXCB::width() const {
  return width_;
}

uint32_t WindowXCB::height() const {
  return height_;
}

const wstring& WindowXCB::title() const {
  return title_;
}

xcb_window_t WindowXCB::window() const {
  return window_;
}
