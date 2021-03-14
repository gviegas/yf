//
// WS
// Delegate.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "Delegate.h"

using namespace WS_NS;

INTERNAL_NS_BEGIN

/// Delegate instance.
///
Delegate deleg{};

INTERNAL_NS_END

WS_NS_BEGIN

const Delegate& delegate() {
  return deleg;
}

void setDelegate(const WdDelegate& delegate) {
  deleg.set(delegate);
}

void setDelegate(const KbDelegate& delegate) {
  deleg.set(delegate);
}

void setDelegate(const PtDelegate& delegate) {
  deleg.set(delegate);
}

WS_NS_END
