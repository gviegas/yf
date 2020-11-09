//
// cg
// Buffer.h
//
// Copyright © 2020 Gustavo C. Viegas.
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
  explicit Buffer(uint64_t size);
  virtual ~Buffer();

  /// Writes data to buffer.
  ///
  virtual void write(uint64_t offset, uint64_t size, const void* data) = 0;

  /// The size of the buffer.
  ///
  const uint64_t size;
};

CG_NS_END

#endif // YF_CG_BUFFER_H
