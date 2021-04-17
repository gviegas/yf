//
// WS
// Platform.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_WS_PLATFORM_H
#define YF_WS_PLATFORM_H

#include "yf/ws/Defs.h"

#if defined(__linux__) // TODO: other unix systems
# include <xcb/xcb.h>
#elif defined(__APPLE__)
# error "Unimplemented"
#elif defined(_WIN32)
# error "Unimplemented"
#else
# error "Invalid platform"
#endif

WS_NS_BEGIN

/// Platforms.
///
enum Platform {
  PlatformNone,
  PlatformWL,
  PlatformW32,
  PlatformXCB,
  PlatformMAC
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
xcb_window_t windowXCB(const Window* window);

#elif defined(__APPLE__)
# error "Unimplemented"

#elif defined(_WIN32)
# error "Unimplemented"

#else
# error "Invalid platform"
#endif // defined(__linux__)

WS_NS_END

#endif // YF_WS_PLATFORM_H
