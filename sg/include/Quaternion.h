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

  /// Construction from a 4-component vector representation (x, y, z, w),
  /// with w as the real part.
  ///
  constexpr explicit Quaternion(const Vector<T, 4>& q)
    : r_(q[3]), v_{q[0], q[1], q[2]} { }

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

  /// Quaternion as a 4-component vector (x, y, z, w), with w as the real part.
  ///
  constexpr Vector<T, 4> q() const {
    return Vector<T, 4>{v_[0], v_[1], v_[2], r_};
  }

  /// In-place multiplication.
  ///
  constexpr Quaternion<T>& operator*=(const Quaternion<T>& other) {
    const T r1 = r_;
    r_ = r1 * other.r_ - dot(v_, other.v_);
    v_ = v_ * other.r_ + other.v_ * r1 + cross(v_, other.v_);
    return *this;
  }

 private:
  T r_{};
  Vector<T, 3> v_{};
};

/// Quaternion multiplication.
///
template<class T>
constexpr Quaternion<T> operator*(const Quaternion<T>& left,
                                  const Quaternion<T>& right) {
  const auto r1 = left.r();
  const auto r2 = right.r();
  const auto& v1 = left.v();
  const auto& v2 = right.v();

  return {r1 * r2 - dot(v1, v2), v1 * r2 + v2 * r1 + cross(v1, v2)};
}

/// Quaternion rotation.
///
template<class T>
constexpr Quaternion<T> rotateQ(T angle, const Vector<T, 3>& axis) {
  const T a = angle * 0.5;
  const T c = std::cos(a);
  const T s = std::sin(a);

  return {c, normalize(axis) * s};
}

/// Quaternion rotation (x-axis).
///
template<class T>
constexpr Quaternion<T> rotateQX(T angle) {
  const T a = angle * 0.5;
  const T c = std::cos(a);
  const T s = std::sin(a);

  return {c, {s, 0.0, 0.0}};
}

/// Quaternion rotation (y-axis).
///
template<class T>
constexpr Quaternion<T> rotateQY(T angle) {
  const T a = angle * 0.5;
  const T c = std::cos(a);
  const T s = std::sin(a);

  return {c, {0.0, s, 0.0}};
}

/// Quaternion rotation (z-axis).
///
template<class T>
constexpr Quaternion<T> rotateQZ(T angle) {
  const T a = angle * 0.5;
  const T c = std::cos(a);
  const T s = std::sin(a);

  return {c, {0.0, 0.0, s}};
}

/// Single precision quaternion.
///
using Qnionf = Quaternion<float>;

/// Double precision quaternion.
///
using Qniond = Quaternion<double>;

SG_NS_END

#endif // YF_SG_QUATERNION_H
