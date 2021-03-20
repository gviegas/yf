//
// SG
// Vector.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_SG_VECTOR_H
#define YF_SG_VECTOR_H

#include <cstdint>
#include <cmath>
#include <algorithm>
#include <initializer_list>
#include <type_traits>

#include "yf/sg/Defs.h"

SG_NS_BEGIN

/// Vector.
///
template<class T, size_t sz>
class Vector {
  static_assert(sz > 0, "Zero-sized Vector not supported");
  static_assert(std::is_arithmetic<T>(), "Vector must be a numeric type");

 public:
  Vector() = default;
  Vector(const Vector&) = default;
  Vector& operator=(const Vector&) = default;
  ~Vector() = default;

  /// Initializer-list construction.
  ///
  /// Missing components are default-initialized.
  ///
  constexpr Vector(std::initializer_list<T> list) {
    for (size_t i = 0; i < std::min(sz, list.size()); ++i)
      data_[i] = *(list.begin()+i);
  }

  constexpr const T& operator[](size_t index) const {
    return data_[index];
  }

  constexpr T& operator[](size_t index) {
    return data_[index];
  }

  constexpr const T* begin() const {
    return data_;
  }

  constexpr T* begin() {
    return data_;
  }

  constexpr const T* end() const {
    return data_+sz;
  }

  constexpr T* end() {
    return data_+sz;
  }

  constexpr const T* data() const {
    return data_;
  }

  constexpr T* data() {
    return data_;
  }

  /// Number of components in the vector.
  ///
  constexpr size_t size() const {
    return sz;
  }

  /// In-place subtraction.
  ///
  constexpr Vector& operator-=(const Vector& other) {
    for (size_t i = 0; i < sz; ++i)
      data_[i] -= other.data_[i];
    return *this;
  }

  /// In-place addition.
  ///
  constexpr Vector& operator+=(const Vector& other) {
    for (size_t i = 0; i < sz; ++i)
      data_[i] += other.data_[i];
    return *this;
  }

  /// In-place multiplication.
  ///
  constexpr Vector& operator*=(T scalar) {
    for (size_t i = 0; i < sz; ++i)
      data_[i] *= scalar;
    return *this;
  }

  /// In-place division.
  ///
  constexpr Vector& operator/=(T scalar) {
    for (size_t i = 0; i < sz; ++i)
      data_[i] /= scalar;
    return *this;
  }

  /// Vector length.
  ///
  constexpr T length() const {
    return std::sqrt(dot(*this, *this));
  }

  /// In-place normalization.
  ///
  constexpr Vector& normalize() {
    return operator/=(length());
  }

 private:
  T data_[sz]{};
};

/// Vector subtraction.
///
template<class T, size_t sz>
constexpr Vector<T, sz> operator-(const Vector<T, sz>& left,
                                  const Vector<T, sz>& right) {
  Vector<T, sz> res;
  for (size_t i = 0; i < sz; ++i)
    res[i] = left[i] - right[i];
  return res;
}

/// Vector addition.
///
template<class T, size_t sz>
constexpr Vector<T, sz> operator+(const Vector<T, sz>& left,
                                  const Vector<T, sz>& right) {
  Vector<T, sz> res;
  for (size_t i = 0; i < sz; ++i)
    res[i] = left[i] + right[i];
  return res;
}

/// Vector multiplication.
///
template<class T, size_t sz>
constexpr Vector<T, sz> operator*(const Vector<T, sz>& vector, T scalar) {
  auto res = vector;
  res *= scalar;
  return res;
}

/// Vector division.
///
template<class T, size_t sz>
constexpr Vector<T, sz> operator/(const Vector<T, sz>& vector, T scalar) {
  auto res = vector;
  res /= scalar;
  return res;
}

/// Vector normalization.
///
template<class T, size_t sz>
constexpr Vector<T, sz> normalize(const Vector<T, sz>& vector) {
  return Vector<T, sz>(vector).normalize();
}

/// Dot product.
///
template<class T, size_t sz>
constexpr T dot(const Vector<T, sz>& v1, const Vector<T, sz>& v2) {
  T res = 0;
  for (size_t i = 0; i < sz; ++i)
    res += v1[i] * v2[i];
  return res;
}

/// Cross product (3-component).
///
template<class T>
constexpr Vector<T, 3> cross(const Vector<T, 3>& v1, const Vector<T, 3>& v2) {
  return {v1[1] * v2[2] - v1[2] * v2[1],
          v1[2] * v2[0] - v1[0] * v2[2],
          v1[0] * v2[1] - v1[1] * v2[0]};
}

/// Cross product (4-component).
///
template<class T>
constexpr Vector<T, 4> cross(const Vector<T, 4>& v1, const Vector<T, 4>& v2) {
  return {v1[1] * v2[2] - v1[2] * v2[1],
          v1[2] * v2[0] - v1[0] * v2[2],
          v1[0] * v2[1] - v1[1] * v2[0],
          1};
}

/// Integer vectors.
///
using Vec2i = Vector<int32_t, 2>;
using Vec3i = Vector<int32_t, 3>;
using Vec4i = Vector<int32_t, 4>;

/// Unsigned integer vectors.
///
using Vec2u = Vector<uint32_t, 2>;
using Vec3u = Vector<uint32_t, 3>;
using Vec4u = Vector<uint32_t, 4>;

/// Single precision vectors.
///
using Vec2f = Vector<float, 2>;
using Vec3f = Vector<float, 3>;
using Vec4f = Vector<float, 4>;

/// Double precision vectors.
///
using Vec2d = Vector<double, 2>;
using Vec3d = Vector<double, 3>;
using Vec4d = Vector<double, 4>;

SG_NS_END

#endif // YF_SG_VECTOR_H
