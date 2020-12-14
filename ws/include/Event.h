//
// WS
// Event.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_WS_EVENT_H
#define YF_WS_EVENT_H

#include <memory>

#include "yf/ws/Defs.h"

WS_NS_BEGIN

struct WdDelegate;
struct KbDelegate;
struct PtDelegate;

/// Event manager.
///
class Event {
 public:
  Event();
  virtual ~Event();

  /// Gets the event instance.
  ///
  static Event& get();

  /// Dispatches events.
  ///
  virtual void dispatch() = 0;

  /// Sets event delegates.
  ///
  void setDelegate(const WdDelegate& delegate);
  void setDelegate(const KbDelegate& delegate);
  void setDelegate(const PtDelegate& delegate);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

WS_NS_END

#endif // YF_WS_EVENT_H
