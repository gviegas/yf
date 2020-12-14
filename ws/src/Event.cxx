//
// WS
// Event.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Event.h"

WS_NS_BEGIN

Event::~Event() { }

// `Platform` provides the implementation.
Event& getEvent();

Event& Event::get() {
  return getEvent();
}

WS_NS_END
