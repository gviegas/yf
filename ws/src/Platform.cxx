//
// WS
// Platform.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Platform.h"
#include "yf/Except.h"

#if defined(__linux__) // TODO: add other unix systems here
//# include "unix/WindowWL.h"
//# include "unix/EventWL.h"
# include "unix/WindowXCB.h"
# include "unix/EventXCB.h"
#elif defined(__APPLE__)
# include "macos/WindowMAC.h"
# include "macos/EventMAC.h"
#elif defined(_WIN32)
# include "win32/WindowW32.h"
# include "win32/EventW32.h"
#else
# error "Invalid platform"
#endif // defined(__linux__)

using namespace std;

INTERNAL_NS_BEGIN

/// The current platform.
///
auto curPfm = WS_NS::PlatformNone;

INTERNAL_NS_END

WS_NS_BEGIN

/// Sets the current platform.
///
/// System-specific `init*()` will call this function.
///
void setPlatform(Platform pfm) {
  curPfm = pfm;
}

/// Makes a new window.
///
/// `Window::make()` will call this function.
///
Window::Ptr makeWindow(uint32_t width, uint32_t height, const wstring& title,
                       Window::CreationMask mask) {

#if defined(__linux__)
  switch (platform()) {
  case PlatformNone:
    throw UnsupportedExcept("No supported platform available");
  case PlatformXCB:
    return make_unique<WindowXCB>(width, height, title, mask);
  default:
    throw runtime_error("Unexpected");
  }
#else
// TODO: other systems
#endif

  return nullptr;
}

/// Gets the event instance.
///
/// `Event::get()` will call this function.
///
Event& getEvent() {
#if defined(__linux__)
  switch (platform()) {
  case PlatformNone:
    throw UnsupportedExcept("No supported platform available");
  case PlatformXCB:
    return EventXCB::get();
  default:
    throw runtime_error("Unexpected");
  }
#else
// TODO: other systems
#endif
}

Platform platform() {
  // Try to initialize a platform if have none
  if (curPfm == PlatformNone) {
#if defined(__linux__)
    if (getenv("WAYLAND_DISPLAY"))
      // TODO: replace with `initWL` when implemented
      initXCB();
    else if (getenv("DISPLAY"))
      initXCB();
#else
// TODO: other systems
#endif
  }

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
#else
// TODO: other systems
#endif

WS_NS_END
