//
// yf
// CGResult.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_RESULT_H
#define YF_CG_RESULT_H

#include <utility>

#include "YFDefs.h"

YF_NS_BEGIN

/// Result type representing success/failure.
///
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

/// Pair holding an arbitrary object plus a CGResult.
///
template <class T>
struct CGResultPair {
  explicit CGResultPair(const T& object, CGResult result = CGResult::Success)
    : object(object), result(result) {}

  explicit CGResultPair(T&& object, CGResult result = CGResult::Success)
    : object(std::move(object)), result(result) {}

  T object;
  CGResult result;
};

YF_NS_END

#endif // YF_CG_RESULT_H
