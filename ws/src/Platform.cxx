//
// WS
// Platform.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Platform.h"
#include "unix/WindowXCB.h"
#include "yf/Except.h"

using namespace std;

INTERNAL_NS_BEGIN

/// The current platform.
///
auto curPfm = WS_NS::PlatformNone;

INTERNAL_NS_END

WS_NS_BEGIN

/// Sets the current platform.
///
void setPlatform(Platform pfm) {
  curPfm = pfm;
}

Platform platform() {
  return curPfm;
}

#if defined(__linux__)
xcb_connection_t* connectionXCB() {
  if (curPfm != PlatformXCB)
    throw runtime_error("XCB is not the current platform");

  return varsXCB().connection;
}

xcb_visualid_t visualIdXCB() {
  if (curPfm != PlatformXCB)
    throw runtime_error("XCB is not the current platform");

  return varsXCB().visualId;
}

xcb_window_t windowXCB(const Window* window) {
  if (curPfm != PlatformXCB)
    throw runtime_error("XCB is not the current platform");

  return static_cast<const WindowXCB*>(window)->window();
}
// TODO: other systems
#endif

WS_NS_END
