//
// WS
// Platform.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#ifndef YF_WS_PLATFORM_H
#define YF_WS_PLATFORM_H

#include "yf/ws/Defs.h"

#if defined(__linux__) // TODO: Other unix systems
# include <xcb/xcb.h>
#elif defined(__APPLE__)
# error "Not implemented"
#elif defined(_WIN32)
# error "Not implemented"
#else
# error "Invalid platform"
#endif

WS_NS_BEGIN

/// Platforms.
///
enum class Platform {
  None,
  Wayland,
  Win32,
  Xcb,
  Macos,
};

/// Identifies the platform at runtime.
///
Platform platform();

class Window;

/// Platform-specific getters.
///
#if defined(__linux__)
xcb_connection_t* connectionXCB();
xcb_visualid_t visualIdXCB();
xcb_window_t windowXCB(const Window& window);

#elif defined(__APPLE__)
# error "Not implemented"

#elif defined(_WIN32)
# error "Not implemented"

#else
# error "Invalid platform"
#endif // defined(__linux__)

WS_NS_END

#endif // YF_WS_PLATFORM_H
