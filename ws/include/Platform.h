//
// WS
// Platform.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_WS_PLATFORM_H
#define YF_WS_PLATFORM_H

#include "yf/ws/Defs.h"

WS_NS_BEGIN

/// Platforms.
///
enum Platform {
  // TODO: other platforms
  PlatformNone,
  PlatformXCB
};

/// Identifies the platform at runtime.
///
Platform platform();

WS_NS_END

#endif // YF_WS_PLATFORM_H
