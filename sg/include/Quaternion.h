//
// SG
// Quaternion.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_QUATERNION_H
#define YF_SG_QUATERNION_H

#include <cmath>
#include <type_traits>

#include "yf/sg/Defs.h"
#include "yf/sg/Vector.h"

SG_NS_BEGIN

/// Quaternion.
///
template<class T>
class Quaternion {
  static_assert(std::is_floating_point<T>(),
                "Quaternion must be a floating-point type");

 public:
  Quaternion() = default;
  Quaternion(const Quaternion&) = default;
  Quaternion& operator=(const Quaternion&) = default;

  /// Construction from real and vector parts (r, v).
  ///
  constexpr Quaternion(T r, const Vector<T, 3>& v) : r_(r), v_(v) { }

  constexpr const T& r() const {
    return r_;
  }

  constexpr T& r() {
    return r_;
  }

  constexpr const Vector<T, 3>& v() const {
    return v_;
  }

  constexpr Vector<T, 3>& v() {
    return v_;
  }

 private:
  T r_{};
  Vector<T, 3> v_{};
};

SG_NS_END

#endif // YF_SG_QUATERNION_H
