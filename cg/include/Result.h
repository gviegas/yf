//
// cg
// Result.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_RESULT_H
#define YF_CG_RESULT_H

#include <utility>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

/// Result type representing success/failure.
///
struct Result {
  /// Result values.
  ///
  enum Value {
    Success,
    Failure
    // TODO
  };

  Result(Value value) : value(value) {}

  operator bool() const {
    return value == Success;
  }

  bool operator==(Result other) const {
    return value == other.value;
  }

  /// The result value.
  ///
  const Value value;
};

/// Pair holding an arbitrary object plus a `Result`.
///
template <class T>
struct ResultPair {
  explicit ResultPair(const T& object, Result result = Result::Success)
    : object(object), result(result) {}

  explicit ResultPair(T&& object, Result result = Result::Success)
    : object(std::move(object)), result(result) {}

  /// Object & result.
  ///
  T object;
  Result result;
};

CG_NS_END

#endif // YF_CG_RESULT_H
