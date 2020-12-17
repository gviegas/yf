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

  static constexpr Matrix<T, colN, colN> identity() {
    Matrix mat;
    for (size_t i = 0; i < colN; ++i)
      mat[i][i] = 1;
    return mat;
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

  constexpr Matrix& operator-=(const Matrix& other) {
    for (size_t i = 0; i < colN; ++i)
      data_[i] -= other[i];
    return *this;
  }

  constexpr Matrix& operator+=(const Matrix& other) {
    for (size_t i = 0; i < colN; ++i)
      data_[i] += other[i];
    return *this;
  }

  constexpr Matrix<T, colN, colN>&
  operator*=(const Matrix<T, colN, colN>& other) {
    const auto tmp = *this;
    for (size_t i = 0; i < colN; ++i) {
      for (size_t j = 0; j < colN; ++j) {
        data_[i][j] = 0;
        for (size_t k = 0; k < colN; ++k)
          data_[i][j] += tmp[k][j] * other[i][k];
      }
    }
    return *this;
  }

  constexpr Matrix<T, colN, colN>& transpose() {
    for (size_t i = 0; i < colN; ++i) {
      for (size_t j = i+1; j < colN; ++j)
        std::swap(data_[i][j], data_[j][i]);
    }
    return *this;
  }

 private:
  Vector<T, rowN> data_[colN]{};
};

template<class T, size_t colN, size_t rowN>
constexpr Matrix<T, colN, rowN> operator-(const Matrix<T, colN, rowN>& left,
                                          const Matrix<T, colN, rowN>& right) {
  Matrix<T, colN, rowN> res;
  for (size_t i = 0; i < colN; ++i)
    res[i] = left[i] - right[i];
  return res;
}

template<class T, size_t colN, size_t rowN>
constexpr Matrix<T, colN, rowN> operator+(const Matrix<T, colN, rowN>& left,
                                          const Matrix<T, colN, rowN>& right) {
  Matrix<T, colN, rowN> res;
  for (size_t i = 0; i <colN; ++i)
    res[i] = left[i] + right[i];
  return res;
}

template<class T, size_t sqN>
constexpr Matrix<T, sqN, sqN> operator*(const Matrix<T, sqN, sqN>& m1,
                                        const Matrix<T, sqN, sqN>& m2) {
  Matrix<T, sqN, sqN> res;
  for (size_t i = 0; i < sqN; ++i) {
    for (size_t j = 0; j < sqN; ++j) {
      for (size_t k = 0; k < sqN; ++k)
        res[i][j] += m1[k][j] * m2[i][k];
    }
  }
  return res;
}

template<class T, size_t sqN>
constexpr Vector<T, sqN> operator*(const Matrix<T, sqN, sqN>& mat,
                                   const Vector<T, sqN>& vec) {
  Vector<T, sqN> res;
  for (size_t i = 0; i < sqN; ++i) {
    for (size_t j = 0; j < sqN; ++j)
      res[i] += mat[j][i] * vec[j];
  }
  return res;
}

template<class T, size_t sqN>
constexpr Matrix<T, sqN, sqN> transpose(const Matrix<T, sqN, sqN>& mat) {
  Matrix<T, sqN, sqN> res;
  for (size_t i = 0; i < sqN; ++i) {
    res[i][i] = mat[i][i];
    for (size_t j = i+1; j < sqN; ++j) {
      res[i][j] = mat[j][i];
      res[j][i] = mat[i][j];
    }
  }
  return res;
}

template<class T>
constexpr Matrix<T, 2, 2> invert(const Matrix<T, 2, 2>& mat) {
  static_assert(std::is_floating_point<T>(),
                "Matrix inversion requires a floating point type");

  const T idet = 1.0 / (mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]);
  return {{mat[1][1]*idet, mat[0][1]*idet}, {-mat[1][0]*idet, mat[0][0]*idet}};
}

template<class T>
constexpr Matrix<T, 3, 3> invert(const Matrix<T, 3, 3>& mat) {
  static_assert(std::is_floating_point<T>(),
                "Matrix inversion requires a floating point type");

  Matrix<T, 3, 3> res;

  const T s0 = mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1];
  const T s1 = mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0];
  const T s2 = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];
  const T idet = 1.0 / (mat[0][0]*s0 - mat[0][1]*s1 + mat[0][2]*s2);

  res[0][0] = +s0 * idet;
  res[0][1] = -(mat[0][1] * mat[2][2] - mat[0][2] * mat[2][1]) * idet;
  res[0][2] = +(mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1]) * idet;
  res[1][0] = -s1 * idet;
  res[1][1] = +(mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0]) * idet;
  res[1][2] = -(mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0]) * idet;
  res[2][0] = +s2 * idet;
  res[2][1] = -(mat[0][0] * mat[2][1] - mat[0][1] * mat[2][0]) * idet;
  res[2][2] = +(mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]) * idet;

  return res;
}

template<class T>
constexpr Matrix<T, 4, 4> invert(const Matrix<T, 4, 4>& mat) {
  static_assert(std::is_floating_point<T>(),
                "Matrix inversion requires a floating point type");

  Matrix<T, 4, 4> res;

  const T s0 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
  const T s1 = mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0];
  const T s2 = mat[0][0] * mat[1][3] - mat[0][3] * mat[1][0];
  const T s3 = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
  const T s4 = mat[0][1] * mat[1][3] - mat[0][3] * mat[1][1];
  const T s5 = mat[0][2] * mat[1][3] - mat[0][3] * mat[1][2];
  const T c0 = mat[2][0] * mat[3][1] - mat[2][1] * mat[3][0];
  const T c1 = mat[2][0] * mat[3][2] - mat[2][2] * mat[3][0];
  const T c2 = mat[2][0] * mat[3][3] - mat[2][3] * mat[3][0];
  const T c3 = mat[2][1] * mat[3][2] - mat[2][2] * mat[3][1];
  const T c4 = mat[2][1] * mat[3][3] - mat[2][3] * mat[3][1];
  const T c5 = mat[2][2] * mat[3][3] - mat[2][3] * mat[3][2];
  const T idet = 1.0 / (s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0);

  res[0][0] = (+c5 * mat[1][1] - c4 * mat[1][2] + c3 * mat[1][3]) * idet;
  res[0][1] = (-c5 * mat[0][1] + c4 * mat[0][2] - c3 * mat[0][3]) * idet;
  res[0][2] = (+s5 * mat[3][1] - s4 * mat[3][2] + s3 * mat[3][3]) * idet;
  res[0][3] = (-s5 * mat[2][1] + s4 * mat[2][2] - s3 * mat[2][3]) * idet;
  res[1][0] = (-c5 * mat[1][0] + c2 * mat[1][2] - c1 * mat[1][3]) * idet;
  res[1][1] = (+c5 * mat[0][0] - c2 * mat[0][2] + c1 * mat[0][3]) * idet;
  res[1][2] = (-s5 * mat[3][0] + s2 * mat[3][2] - s1 * mat[3][3]) * idet;
  res[1][3] = (+s5 * mat[2][0] - s2 * mat[2][2] + s1 * mat[2][3]) * idet;
  res[2][0] = (+c4 * mat[1][0] - c2 * mat[1][1] + c0 * mat[1][3]) * idet;
  res[2][1] = (-c4 * mat[0][0] + c2 * mat[0][1] - c0 * mat[0][3]) * idet;
  res[2][2] = (+s4 * mat[3][0] - s2 * mat[3][1] + s0 * mat[3][3]) * idet;
  res[2][3] = (-s4 * mat[2][0] + s2 * mat[2][1] - s0 * mat[2][3]) * idet;
  res[3][0] = (-c3 * mat[1][0] + c1 * mat[1][1] - c0 * mat[1][2]) * idet;
  res[3][1] = (+c3 * mat[0][0] - c1 * mat[0][1] + c0 * mat[0][2]) * idet;
  res[3][2] = (-s3 * mat[3][0] + s1 * mat[3][1] - s0 * mat[3][2]) * idet;
  res[3][3] = (+s3 * mat[2][0] - s1 * mat[2][1] + s0 * mat[2][2]) * idet;

  return res;
}

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
