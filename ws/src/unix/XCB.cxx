//
// WS
// XCB.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <dlfcn.h>
#define WS_LIBXCB "libxcb.so"

#include "XCB.h"
#include "yf/Except.h"

using namespace WS_NS;
using namespace std;

INTERNAL_NS_BEGIN

/// Lib handle.
///
void* libHandle = nullptr;

/// Loads XCB lib and set dynamic symbols.
///
void loadXCB() {
  if (libHandle)
    return;

  void* handle = dlopen(WS_LIBXCB, RTLD_LAZY);
  if (!handle)
    throw runtime_error("Could not open XCB lib");

#define WS_SETFP(fp, name) do { \
fp = reinterpret_cast<decltype(fp)>(dlsym(handle, name)); \
if (!fp) { \
  dlclose(handle); \
  throw runtime_error("Could not load XCB symbol: " name); \
} } while (0)

  WS_SETFP(connectXCB, "xcb_connect");
  WS_SETFP(disconnectXCB, "xcb_disconnect");
  WS_SETFP(flushXCB, "xcb_flush");
  WS_SETFP(connectionHasErrorXCB, "xcb_connection_has_error");
  WS_SETFP(generateIdXCB, "xcb_generate_id");
  WS_SETFP(pollForEventXCB, "xcb_poll_for_event");
  WS_SETFP(requestCheckXCB, "xcb_request_check");
  WS_SETFP(getSetupXCB, "xcb_get_setup");
  WS_SETFP(setupRootsIteratorXCB, "xcb_setup_roots_iterator");
  WS_SETFP(createWindowCheckedXCB, "xcb_create_window_checked");
  WS_SETFP(destroyWindowXCB, "xcb_destroy_window");
  WS_SETFP(mapWindowCheckedXCB, "xcb_map_window_checked");
  WS_SETFP(unmapWindowCheckedXCB, "xcb_unmap_window_checked");
  WS_SETFP(configureWindowCheckedXCB, "xcb_configure_window_checked");
  WS_SETFP(internAtomXCB, "xcb_intern_atom");
  WS_SETFP(internAtomReplyXCB, "xcb_intern_atom_reply");
  WS_SETFP(changePropertyCheckedXCB, "xcb_change_property_checked");
  WS_SETFP(changeKeyboardControlCheckedXCB,
  "xcb_change_keyboard_control_checked");

#undef WS_SETFP

  libHandle = handle;
}

/// Unloads XCB lib.
///
void unloadXCB() {
  if (libHandle) {
    dlclose(libHandle);
    libHandle = nullptr;
  }
}

INTERNAL_NS_END

void WS_NS::initXCB() {
  loadXCB();

  // TODO...
}

void WS_NS::deinitXCB() {
  unloadXCB();

  // TODO...
}

WS_NS_BEGIN

xcb_connection_t
(*connectXCB)(const char*, int*);

void
(*disconnectXCB)(xcb_connection_t*);

int
(*flushXCB)(xcb_connection_t*);

int
(*connectionHasErrorXCB)(xcb_connection_t*);

uint32_t
(*generateIdXCB)(xcb_connection_t*);

xcb_generic_event_t*
(*pollForEventXCB)(xcb_connection_t*);

xcb_generic_error_t*
(*requestCheckXCB)(xcb_connection_t*, xcb_void_cookie_t);

const struct scb_setup_t*
(*getSetupXCB)(xcb_connection_t*);

xcb_screen_iterator_t
(*setupRootsIteratorXCB)(const xcb_setup_t*);

xcb_void_cookie_t
(*createWindowCheckedXCB)(xcb_connection_t*, uint8_t, xcb_window_t,
                          xcb_window_t, int16_t, int16_t, uint16_t,
                          uint16_t, uint16_t, uint16_t, xcb_visualid_t,
                          uint32_t, const void*);

xcb_void_cookie_t
(*destroyWindowXCB)(xcb_connection_t*, xcb_window_t);

xcb_void_cookie_t
(*mapWindowCheckedXCB)(xcb_connection_t*, xcb_window_t);

xcb_void_cookie_t
(*unmapWindowCheckedXCB)(xcb_connection_t*, xcb_window_t);

xcb_void_cookie_t
(*configureWindowCheckedXCB)(xcb_connection_t*, xcb_window_t, uint32_t,
                             const void*);

xcb_intern_atom_cookie_t
(*internAtomXCB)(xcb_connection_t*, uint8_t, uint16_t, const char*);

xcb_intern_atom_reply_t*
(*internAtomReplyXCB)(xcb_connection_t*, xcb_intern_atom_cookie_t,
                      xcb_generic_error_t**);

xcb_void_cookie_t
(*changePropertyCheckedXCB)(xcb_connection_t*, uint8_t, xcb_window_t,
                            xcb_atom_t, xcb_atom_t, uint8_t, uint32_t,
                            const void*);

xcb_void_cookie_t
(*changeKeyboardControlCheckedXCB)(xcb_connection_t*, uint32_t, const void*);

WS_NS_END
