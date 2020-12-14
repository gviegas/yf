//
// WS
// EventXCB.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_WS_EVENTXCB_H
#define YF_WS_EVENTXCB_H

#include "Event.h"
#include "Window.h"
#include "Keyboard.h"
#include "Pointer.h"

WS_NS_BEGIN

class EventXCB final : public Event {
 public:
  ~EventXCB() = default;
  static EventXCB& get();
  void dispatch();

 private:
  EventXCB() = default;
};

WS_NS_END

#endif // YF_WS_EVENTXCB_H
