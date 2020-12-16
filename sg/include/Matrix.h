//
// SG
// Matrix.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_MATRIX_H
#define YF_SG_MATRIX_H

#include <cstdint>
#include <cmath>
#include <algorithm>
#include <initializer_list>
#include <type_traits>

#include "yf/sg/Defs.h"
#include "yf/sg/Vector.h"

SG_NS_BEGIN

template<class T, size_t colN, size_t rowN>
class Matrix {
  static_assert(colN > 0 && rowN > 0, "Zero-sized Matrix not supported");
  static_assert(std::is_arithmetic<T>(), "Matrix must be a numeric type");

 public:
  Matrix() = default;

  Matrix(std::initializer_list<Vector<T, rowN>> list) {
    for (size_t i = 0; i < std::min(colN, list.size()); ++i)
      data_[i] = *(list.begin()+i);
  }

  constexpr const Vector<T, rowN>& operator[](size_t column) const {
    return data_[column];
  }

  constexpr Vector<T, rowN>& operator[](size_t column) {
    return data_[column];
  }

  constexpr const Vector<T, rowN>* begin() const {
    return data_;
  }

  constexpr Vector<T, rowN>* begin() {
    return data_;
  }

  constexpr const Vector<T, rowN>* end() const {
    return data_+colN;
  }

  constexpr Vector<T, rowN>* end() {
    return data_+colN;
  }

  constexpr size_t columns() const {
    return colN;
  }

  constexpr size_t rows() const {
    return rowN;
  }

 private:
  Vector<T, rowN> data_[colN]{};
};

using Mat2i   = Matrix<int32_t, 2, 2>;
using Mat2x3i = Matrix<int32_t, 2, 3>;
using Mat2x4i = Matrix<int32_t, 2, 4>;
using Mat3x2i = Matrix<int32_t, 3, 2>;
using Mat3i   = Matrix<int32_t, 3, 3>;
using Mat3x4i = Matrix<int32_t, 3, 4>;
using Mat4x2i = Matrix<int32_t, 4, 2>;
using Mat4x3i = Matrix<int32_t, 4, 3>;
using Mat4i   = Matrix<int32_t, 4, 4>;

using Mat2u   = Matrix<uint32_t, 2, 2>;
using Mat2x3u = Matrix<uint32_t, 2, 3>;
using Mat2x4u = Matrix<uint32_t, 2, 4>;
using Mat3x2u = Matrix<uint32_t, 3, 2>;
using Mat3u   = Matrix<uint32_t, 3, 3>;
using Mat3x4u = Matrix<uint32_t, 3, 4>;
using Mat4x2u = Matrix<uint32_t, 4, 2>;
using Mat4x3u = Matrix<uint32_t, 4, 3>;
using Mat4u   = Matrix<uint32_t, 4, 4>;

using Mat2f   = Matrix<float, 2, 2>;
using Mat2x3f = Matrix<float, 2, 3>;
using Mat2x4f = Matrix<float, 2, 4>;
using Mat3x2f = Matrix<float, 3, 2>;
using Mat3f   = Matrix<float, 3, 3>;
using Mat3x4f = Matrix<float, 3, 4>;
using Mat4x2f = Matrix<float, 4, 2>;
using Mat4x3f = Matrix<float, 4, 3>;
using Mat4f   = Matrix<float, 4, 4>;

using Mat2d   = Matrix<double, 2, 2>;
using Mat2x3d = Matrix<double, 2, 3>;
using Mat2x4d = Matrix<double, 2, 4>;
using Mat3x2d = Matrix<double, 3, 2>;
using Mat3d   = Matrix<double, 3, 3>;
using Mat3x4d = Matrix<double, 3, 4>;
using Mat4x2d = Matrix<double, 4, 2>;
using Mat4x3d = Matrix<double, 4, 3>;
using Mat4d   = Matrix<double, 4, 4>;

SG_NS_END

#endif // YF_SG_MATRIX_H
