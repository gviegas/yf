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
#include "XCB.h"

WS_NS_BEGIN

class EventXCB final : public Event {
 public:
  ~EventXCB() = default;

  static EventXCB& get();

  void dispatch();
  void setDelegate(const WdDelegate&);
  void setDelegate(const KbDelegate&);
  void setDelegate(const PtDelegate&);

 private:
  using Mask = uint32_t;
  enum Flags : uint32_t {
    WdClose  = 0x001,
    WdResize = 0x002,
    KbEnter  = 0x004,
    KbLeave  = 0x008,
    KbKey    = 0x010,
    PtEnter  = 0x020,
    PtLeave  = 0x040,
    PtMotion = 0x080,
    PtButton = 0x100
  };

  EventXCB() = default;

  Mask mask_ = 0;
  WdDelegate wdDeleg_{};
  KbDelegate kbDeleg_{};
  PtDelegate ptDeleg_{};
  xcb_window_t window_ = 0;
  int32_t x_ = -1;
  int32_t y_ = -1;
};

WS_NS_END

#endif // YF_WS_EVENTXCB_H
