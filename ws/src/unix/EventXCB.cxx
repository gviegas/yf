//
// WS
// EventXCB.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cassert>

#include "EventXCB.h"

using namespace WS_NS;

EventXCB& EventXCB::get() {
  static EventXCB ev;
  return ev;
}

void EventXCB::dispatch() {
  // TODO
  assert(false);
}

void EventXCB::setDelegate(const WdDelegate& delegate) {
  // TODO
  assert(false);
}

void EventXCB::setDelegate(const KbDelegate& delegate) {
  // TODO
  assert(false);
}

void EventXCB::setDelegate(const PtDelegate& delegate) {
  // TODO
  assert(false);
}
