//
// CG
// Buffer.cxx
//
// Copyright © 2023 Gustavo C. Viegas.
//

#include "Buffer.h"

using namespace CG_NS;

// TODO: Validate parameters here rather than on backend.
Buffer::Buffer(const Desc& desc)
  : size_(desc.size), mode_(desc.mode), usageMask_(desc.usageMask) { }

uint64_t Buffer::size() const {
  return size_;
}

Buffer::Mode Buffer::mode() const {
  return mode_;
}

Buffer::UsageMask Buffer::usageMask() const {
  return usageMask_;
}
