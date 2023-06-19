//
// CG
// Buffer.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#ifndef YF_CG_BUFFER_H
#define YF_CG_BUFFER_H

#include <cstdint>
#include <memory>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

/// Linear array of data in device memory.
///
class Buffer {
 public:
  using Ptr = std::unique_ptr<Buffer>;

  /// Mask of `Usage` bits.
  using UsageMask = uint32_t;

  /// Usages of a buffer.
  enum Usage : uint32_t {
    CopySrc  = 0x01,
    CopyDst  = 0x02,
    Vertex   = 0x04,
    Index    = 0x08,
    Indirect = 0x10,
    Uniform  = 0x20,
    Storage  = 0x40,
    Query    = 0x80
  };

  /// Memory modes.
  enum class Mode {
    Shared,
    Private
  };

  // TODO: Update this when migrating to C++20
#if __cplusplus >= 202002L
# error use `using` instead
#else
  static constexpr Mode Shared = Mode::Shared;
  static constexpr Mode Private = Mode::Private;
#endif

  Buffer() = default;
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  virtual ~Buffer() = default;

  /// Writes data to buffer.
  ///
  virtual void write(uint64_t offset, uint64_t size, const void* data) = 0;

  /// Gets the size of the buffer.
  ///
  virtual uint64_t size() const = 0;
};

CG_NS_END

#endif // YF_CG_BUFFER_H
