//
// CG
// Types.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#ifndef YF_CG_TYPES_H
#define YF_CG_TYPES_H

#include <cstdint>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

namespace detail {
  /// Two-dimensional size.
  ///
  template<class T>
  struct Size2 {
    T width;
    T height;

    Size2(T width, T height) : width(width), height(height) { }

    bool operator==(const Size2& other) const {
      return width == other.width && height == other.height;
    }

    bool operator!=(const Size2& other) const {
      return !operator==(other);
    }
  };

  /// Three-dimensional size.
  ///
  template<class T>
  struct Size3 : Size2<T> {
    T depthOrLayers;

    Size3(T width, T height, T depthOrLayers)
      : Size2<T>(width, height), depthOrLayers(depthOrLayers) { }

    Size3(Size2<T> size2, T depthOrLayers)
      : Size2<T>(size2), depthOrLayers(depthOrLayers) { }

    bool operator==(const Size3& other) const {
      return Size2<T>::operator==(other) &&
             depthOrLayers == other.depthOrLayers;
    }

    bool operator!=(const Size3& other) const {
      return !operator==(other);
    }
  };

  /// Two-dimensional offset.
  ///
  template<class T>
  struct Offset2 {
    T x;
    T y;

    Offset2(T x, T y) : x(x), y(y) { }
    Offset2() : Offset2({}, {}) { }

    bool operator==(const Offset2& other) const {
      return x == other.x && y == other.y;
    }

    bool operator!=(const Offset2& other) const {
      return !operator==(other);
    }
  };

  /// Three-dimensional offset.
  ///
  template<class T>
  struct Offset3 : Offset2<T> {
    T z;

    Offset3(T x, T y, T z) : Offset2<T>(x, y), z(z) { }
    Offset3(Offset2<T> offset2, T z) : Offset2<T>(offset2), z(z) { }
    Offset3() : Offset3({}, {}, {}) { }

    bool operator==(const Offset3& other) const {
      return Offset2<T>::operator==(other) && z == other.z;
    }

    bool operator!=(const Offset3& other) const {
      return !operator==(other);
    }
  };

  /// Range.
  ///
  template<class T>
  struct Range {
    T start; // Inclusive
    T end; // Exclusive

    Range(T start, T end) : start(start), end(end) { }

    bool operator==(const Range& other) const {
      return start == other.start && end == other.end;
    }

    bool operator!=(const Range& other) const {
      return !operator==(other);
    }

    /// Computes `end - start`.
    ///
    /// It assumes that `start` is no greater than `end`.
    ///
    T count() const {
      return end - start;
    }

    /// Checks whether the range contains `other`.
    ///
    /// It assumes that `start` is no greater than `end`.
    ///
    bool contains(const Range& other) const {
      return start <= other.start && end >= other.end;
    }
  };
} // namespace detail

/// Two-dimensional size.
///
using Size2 = detail::Size2<uint32_t>;

/// Three-dimensional size.
///
using Size3 = detail::Size3<uint32_t>;

/// Signed two-dimensional offset.
///
using Offset2 = detail::Offset2<int32_t>;

/// Signed three-dimensional offset.
///
using Offset3 = detail::Offset3<int32_t>;

/// Unsigned two-dimensional origin.
///
using Origin2 = detail::Offset2<uint32_t>;

/// Unsigned three-dimensional origin.
///
using Origin3 = detail::Offset3<uint32_t>;

/// Range.
///
using Range = detail::Range<uint32_t>;

CG_NS_END

#endif // YF_CG_TYPES_H
