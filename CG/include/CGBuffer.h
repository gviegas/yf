//
// yf
// CGBuffer.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_BUFFER_H
#define YF_CG_BUFFER_H

#include <cstdint>
#include <memory>

#include "YFDefs.h"
#include "CGResult.h"

YF_NS_BEGIN

/// Linear array of data in device memory.
///
class CGBuffer {
 public:
  using Ptr = std::unique_ptr<CGBuffer>;
  explicit CGBuffer(uint64_t size);
  virtual ~CGBuffer();

  /// Writes data to buffer.
  ///
  virtual CGResult write(uint64_t offset, uint64_t size, const void* data) = 0;

  /// The size of the buffer.
  ///
  const uint64_t size;
};

YF_NS_END

#endif // YF_CG_BUFFER_H
