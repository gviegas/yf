//
// yf
// CGBuffer.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_BUFFER_H
#define YF_CG_BUFFER_H

#include <cstdint>

#include "YFDefs.h"
#include "CGResult.h"

YF_NS_BEGIN

class CGBuffer {
 public:
  CGBuffer(uint64_t size);
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
