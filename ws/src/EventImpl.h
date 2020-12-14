//
// WS
// EventImpl.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_WS_EVENTIMPL_H
#define YF_WS_EVENTIMPL_H

#include "Event.h"
#include "Window.h"
#include "Keyboard.h"
#include "Pointer.h"

WS_NS_BEGIN

/// Common event implementation.
///
class Event::Impl {
 public:
  /// Mask of `Flags` bits.
  ///
  using Mask = uint32_t;

  /// Flags representing events.
  ///
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

  /// Sets window event delegate and updates event mask accordingly.
  ///
  void setDelegate(const WdDelegate& delegate) {
    wdDeleg_ = delegate;

    mask_ = wdDeleg_.close  ? (mask_ | WdClose)  : (mask_ & ~WdClose);
    mask_ = wdDeleg_.resize ? (mask_ | WdResize) : (mask_ & ~WdResize);
  }

  /// Sets keyboard event delegate and updates event mask accordingly.
  ///
  void setDelegate(const KbDelegate& delegate) {
    kbDeleg_ = delegate;

    mask_ = kbDeleg_.enter ? (mask_ | KbEnter) : (mask_ & ~KbEnter);
    mask_ = kbDeleg_.leave ? (mask_ | KbLeave) : (mask_ & ~KbLeave);
    mask_ = kbDeleg_.key   ? (mask_ | KbKey)   : (mask_ & ~KbKey);
  }

  /// Sets pointer event delegate and updates event mask accordingly.
  ///
  void setDelegate(const PtDelegate& delegate) {
    ptDeleg_ = delegate;

    mask_ = ptDeleg_.enter  ? (mask_ | PtEnter)  : (mask_ & ~PtEnter);
    mask_ = ptDeleg_.leave  ? (mask_ | PtLeave)  : (mask_ & ~PtLeave);
    mask_ = ptDeleg_.motion ? (mask_ | PtMotion) : (mask_ & ~PtMotion);
    mask_ = ptDeleg_.button ? (mask_ | PtButton) : (mask_ & ~PtButton);
  }

  /// Variables for delegation management.
  ///
  /// The base `Event` class calls `setDelegate()` functions to update these
  /// variables as needed.
  /// Event subclasses read from these variables to identify which events
  /// must be dispatched.
  ///
  Mask mask_ = 0;
  WdDelegate wdDeleg_{};
  KbDelegate kbDeleg_{};
  PtDelegate ptDeleg_{};
};

WS_NS_END

#endif // YF_WS_EVENTIMPL_H
