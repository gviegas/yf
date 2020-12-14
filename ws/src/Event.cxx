//
// WS
// Event.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "EventImpl.h"

using namespace std;

WS_NS_BEGIN

Event::Event() : impl_(make_unique<Impl>()) { }

Event::~Event() { }

// `Platform` provides the implementation.
Event& getEvent();

Event& Event::get() {
  return getEvent();
}

void Event::setDelegate(const WdDelegate& delegate) {
  impl_->setDelegate(delegate);
}

void Event::setDelegate(const KbDelegate& delegate) {
  impl_->setDelegate(delegate);
}

void Event::setDelegate(const PtDelegate& delegate) {
  impl_->setDelegate(delegate);
}

WS_NS_END
