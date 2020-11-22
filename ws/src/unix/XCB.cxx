//
// WS
// XCB.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cstring>
#include <cstdlib>
#include <dlfcn.h>
#define WS_LIBXCB "libxcb.so"

#include "XCB.h"
#include "Platform.h"
#include "yf/Except.h"

using namespace WS_NS;
using namespace std;

WS_NS_BEGIN

void setPlatform(Platform);

WS_NS_END

INTERNAL_NS_BEGIN

/// VarsXCB instance.
///
VarsXCB vars{nullptr, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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

const VarsXCB& WS_NS::varsXCB() {
  if (!libHandle)
    initXCB();

  return vars;
}

void WS_NS::initXCB() {
  loadXCB();

  const char protoName[] = "WM_PROTOCOLS";
  const char delName[] = "WM_DELETE_WINDOW";
  const char titleName[] = "WM_NAME";
  const char utf8Name[] = "UTF8_STRING";
  const char className[] = "WM_CLASS";

  xcb_connection_t* conn = nullptr;
  xcb_generic_error_t* err = nullptr;
  xcb_intern_atom_cookie_t atomCookie;
  xcb_intern_atom_reply_t* atomReply = nullptr;
  int res;

  auto deinit = [&] {
    free(err);
    if (conn)
      disconnectXCB(conn);
  };

  // Connect
  conn = connectXCB(nullptr, nullptr);
  res = connectionHasErrorXCB(conn);
  if (res != 0) {
    deinit();
    throw runtime_error("connectXCB failed");
  }

  // Get visualID & root window
  auto setup = getSetupXCB(conn);
  if (!setup) {
    deinit();
    throw runtime_error("getSetupXCB failed");
  }
  auto screenIt = setupRootsIteratorXCB(setup);
  vars.visualId = screenIt.data->root_visual;
  vars.root = screenIt.data->root;
  vars.whitePixel = screenIt.data->white_pixel;
  vars.blackPixel = screenIt.data->black_pixel;

  // Get protocol atom
  atomCookie = internAtomXCB(conn, 0, strlen(protoName), protoName);
  atomReply = internAtomReplyXCB(conn, atomCookie, &err);
  if (err || !atomReply) {
    deinit();
    throw runtime_error("internAtomReplyXCB failed");
  }
  vars.protocolAtom = atomReply->atom;
  free(atomReply);
  atomReply = nullptr;

  // Get delete atom
  atomCookie = internAtomXCB(conn, 0, strlen(delName), delName);
  atomReply = internAtomReplyXCB(conn, atomCookie, &err);
  if (err || !atomReply) {
    deinit();
    throw runtime_error("internAtomReplyXCB failed");
  }
  vars.deleteAtom = atomReply->atom;
  free(atomReply);
  atomReply = nullptr;

  // Get title atom
  atomCookie = internAtomXCB(conn, 0, strlen(titleName), titleName);
  atomReply = internAtomReplyXCB(conn, atomCookie, &err);
  if (err || !atomReply) {
    deinit();
    throw runtime_error("internAtomReplyXCB failed");
  }
  vars.titleAtom = atomReply->atom;
  free(atomReply);
  atomReply = nullptr;

  // Get utf8 atom
  atomCookie = internAtomXCB(conn, 0, strlen(utf8Name), utf8Name);
  atomReply = internAtomReplyXCB(conn, atomCookie, &err);
  if (err || !atomReply) {
    deinit();
    throw runtime_error("internAtomReplyXCB failed");
  }
  vars.utf8Atom = atomReply->atom;
  free(atomReply);
  atomReply = nullptr;

  // Get class atom
  atomCookie = internAtomXCB(conn, 0, strlen(className), className);
  atomReply = internAtomReplyXCB(conn, atomCookie, &err);
  if (err || !atomReply) {
    deinit();
    throw runtime_error("internAtomReplyXCB failed");
  }
  vars.classAtom = atomReply->atom;
  free(atomReply);
  atomReply = nullptr;

  // Disable keyboard auto repeat
  auto valMask = XCB_KB_AUTO_REPEAT_MODE;
  auto valList = XCB_AUTO_REPEAT_MODE_OFF;
  auto cookie = changeKeyboardControlCheckedXCB(conn, valMask, &valList);
  err = requestCheckXCB(conn, cookie);
  if (err) {
    deinit();
    throw runtime_error("changeKeyboardControlCheckedXCB failed");
  }

  // Flush
  res = flushXCB(conn);
  if (res <= 0) {
    deinit();
    throw runtime_error("flushXCB failed");
  }

  vars.connection = conn;

  setPlatform(PlatformXCB);
}

void WS_NS::deinitXCB() {
  if (vars.connection) {
    disconnectXCB(vars.connection);
    memset(&vars, 0, sizeof vars);
  }

  unloadXCB();

  setPlatform(PlatformNone);
}

WS_NS_BEGIN

xcb_connection_t*
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

const struct xcb_setup_t*
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
