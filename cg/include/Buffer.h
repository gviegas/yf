//
// CG
// Buffer.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
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

  Buffer() = default;
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  virtual ~Buffer();

  /// Writes data to buffer.
  ///
  virtual void write(uint64_t offset, uint64_t size, const void* data) = 0;

  /// Gets the size of the buffer.
  ///
  virtual uint64_t size() const = 0;
};

CG_NS_END

#endif // YF_CG_BUFFER_H
