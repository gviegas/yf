//
// yf
// CGResult.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_RESULT_H
#define YF_CG_RESULT_H

#include "YFDefs.h"

YF_NS_BEGIN

struct CGResult {
  // TODO
  enum Value {
    Success,
    Failure
  };

  CGResult(Value value) : value(value) {} // implicit

  // implicit
  operator bool() const {
    return value == Success;
  }

  bool operator==(CGResult other) const {
    return value == other.value;
  }

  const Value value;
};

YF_NS_END

#endif // YF_CG_RESULT_H
