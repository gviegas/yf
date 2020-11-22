//
// WS
// Platform.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Platform.h"

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

WS_NS_END
