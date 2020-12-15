//
// SG
// Vector.h
//
// Copyright © 2020 Gustavo C. Viegas.
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

template<class T, size_t sz>
class Vector {
  static_assert(sz > 0, "Zero-sized Vector not supported");
  static_assert(std::is_arithmetic<T>(), "Vector must be a numeric type");

 public:
  Vector() = default;

  Vector(std::initializer_list<T> list) {
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

  constexpr size_t size() const {
    return sz;
  }

  constexpr Vector& operator-=(const Vector& other) {
    for (size_t i = 0; i < sz; ++i)
      data_[i] -= other.data_[i];
    return *this;
  }

  constexpr Vector& operator+=(const Vector& other) {
    for (size_t i = 0; i < sz; ++i)
      data_[i] += other.data_[i];
    return *this;
  }

  constexpr Vector& operator*=(T scalar) {
    for (size_t i = 0; i < sz; ++i)
      data_[i] *= scalar;
    return *this;
  }

  constexpr Vector& operator/=(T scalar) {
    for (size_t i = 0; i < sz; ++i)
      data_[i] /= scalar;
    return *this;
  }

  constexpr T length() const {
    return std::sqrt(dot(*this, *this));
  }

  constexpr Vector& normalize() {
    return operator/=(length());
  }

 private:
  T data_[sz]{};
};

template<class T, size_t sz>
constexpr Vector<T, sz> operator-(const Vector<T, sz>& left,
                                  const Vector<T, sz>& right) {
  Vector<T, sz> res;
  for (size_t i = 0; i < sz; ++i)
    res[i] = left[i] - right[i];
  return res;
}

template<class T, size_t sz>
constexpr Vector<T, sz> operator+(const Vector<T, sz>& left,
                                  const Vector<T, sz>& right) {
  Vector<T, sz> res;
  for (size_t i = 0; i < sz; ++i)
    res[i] = left[i] + right[i];
  return res;
}

template<class T, size_t sz>
constexpr Vector<T, sz> operator*(const Vector<T, sz>& vector, T scalar) {
  auto res = vector;
  res *= scalar;
  return res;
}

template<class T, size_t sz>
constexpr Vector<T, sz> operator/(const Vector<T, sz>& vector, T scalar) {
  auto res = vector;
  res /= scalar;
  return res;
}

template<class T, size_t sz>
constexpr Vector<T, sz> normalize(const Vector<T, sz>& vector) {
  return Vector<T, sz>(vector).normalize();
}

template<class T, size_t sz>
constexpr T dot(const Vector<T, sz>& v1, const Vector<T, sz>& v2) {
  T res = 0;
  for (size_t i = 0; i < sz; ++i)
    res += v1[i] * v2[i];
  return res;
}

template<class T>
constexpr Vector<T, 3> cross(const Vector<T, 3>& v1, const Vector<T, 3>& v2) {
  return {v1[1] * v2[2] - v2[1] * v1[2],
          v1[2] * v2[0] - v2[2] * v1[0],
          v1[0] * v2[1] - v2[0] * v1[1]};
}

template<class T>
constexpr Vector<T, 4> cross(const Vector<T, 4>& v1, const Vector<T, 4>& v2) {
  return {v1[1] * v2[2] - v2[1] * v1[2],
          v1[2] * v2[0] - v2[2] * v1[0],
          v1[0] * v2[1] - v2[0] * v1[1],
          1};
}

using Vec2i = Vector<int32_t, 2>;
using Vec3i = Vector<int32_t, 3>;
using Vec4i = Vector<int32_t, 4>;

using Vec2u = Vector<uint32_t, 2>;
using Vec3u = Vector<uint32_t, 3>;
using Vec4u = Vector<uint32_t, 4>;

using Vec2f = Vector<float, 2>;
using Vec3f = Vector<float, 3>;
using Vec4f = Vector<float, 4>;

using Vec2d = Vector<double, 2>;
using Vec3d = Vector<double, 3>;
using Vec4d = Vector<double, 4>;

SG_NS_END

#endif // YF_SG_VECTOR_H
