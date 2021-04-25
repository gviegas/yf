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

void onWdClose(WdCloseFn fn) {
  deleg.wdClose_ = fn;
  if (fn)
    deleg.mask_ |= Delegate::WdClose;
  else
    deleg.mask_ &= ~Delegate::WdClose;
}

void onWdResize(WdResizeFn fn) {
  deleg.wdResize_ = fn;
  if (fn)
    deleg.mask_ |= Delegate::WdResize;
  else
    deleg.mask_ &= ~Delegate::WdResize;
}

void onKbEnter(KbEnterFn fn) {
  deleg.kbEnter_ = fn;
  if (fn)
    deleg.mask_ |= Delegate::KbEnter;
  else
    deleg.mask_ &= ~Delegate::KbEnter;
}

void onKbLeave(KbLeaveFn fn) {
  deleg.kbLeave_ = fn;
  if (fn)
    deleg.mask_ |= Delegate::KbLeave;
  else
    deleg.mask_ &= ~Delegate::KbLeave;
}

void onKbKey(KbKeyFn fn) {
  deleg.kbKey_ = fn;
  if (fn)
    deleg.mask_ |= Delegate::KbKey;
  else
    deleg.mask_ &= ~Delegate::KbKey;
}

void onPtEnter(PtEnterFn fn) {
  deleg.ptEnter_ = fn;
  if (fn)
    deleg.mask_ |= Delegate::PtEnter;
  else
    deleg.mask_ &= ~Delegate::PtEnter;
}

void onPtLeave(PtLeaveFn fn) {
  deleg.ptLeave_ = fn;
  if (fn)
    deleg.mask_ |= Delegate::PtLeave;
  else
    deleg.mask_ &= ~Delegate::PtLeave;
}

void onPtMotion(PtMotionFn fn) {
  deleg.ptMotion_ = fn;
  if (fn)
    deleg.mask_ |= Delegate::PtMotion;
  else
    deleg.mask_ &= ~Delegate::PtMotion;
}

void onPtButton(PtButtonFn fn) {
  deleg.ptButton_ = fn;
  if (fn)
    deleg.mask_ |= Delegate::PtButton;
  else
    deleg.mask_ &= ~Delegate::PtButton;
}

WS_NS_END
