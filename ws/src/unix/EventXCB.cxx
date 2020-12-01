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
  wdDeleg_ = delegate;

  mask_ = wdDeleg_.close  ? (mask_ | WdClose)  : (mask_ & ~WdClose);
  mask_ = wdDeleg_.resize ? (mask_ | WdResize) : (mask_ & ~WdResize);
}

void EventXCB::setDelegate(const KbDelegate& delegate) {
  kbDeleg_ = delegate;

  mask_ = kbDeleg_.enter ? (mask_ | KbEnter) : (mask_ & ~KbEnter);
  mask_ = kbDeleg_.leave ? (mask_ | KbLeave) : (mask_ & ~KbLeave);
  mask_ = kbDeleg_.key   ? (mask_ | KbKey)   : (mask_ & ~KbKey);
}

void EventXCB::setDelegate(const PtDelegate& delegate) {
  ptDeleg_ = delegate;

  mask_ = ptDeleg_.enter  ? (mask_ | PtEnter)  : (mask_ & ~PtEnter);
  mask_ = ptDeleg_.leave  ? (mask_ | PtLeave)  : (mask_ & ~PtLeave);
  mask_ = ptDeleg_.motion ? (mask_ | PtMotion) : (mask_ & ~PtMotion);
  mask_ = ptDeleg_.button ? (mask_ | PtButton) : (mask_ & ~PtButton);
}
