//
// SG
// Vector.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_VECTOR_H
#define YF_SG_VECTOR_H

#include <cstdint>
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

 private:
  T data_[sz]{};
};

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
