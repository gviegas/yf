//
// CG
// Queue.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_QUEUE_H
#define YF_CG_QUEUE_H

#include <cstdint>
#include <memory>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

class Queue;
class Encoder;

/// Command buffer.
///
class CmdBuffer {
 public:
  using Ptr = std::unique_ptr<CmdBuffer>;

  virtual ~CmdBuffer();

  /// Encodes the command buffer with the contents of an encoder object.
  ///
  virtual void encode(const Encoder& encoder) = 0;

  /// Enqueues the command buffer for execution.
  ///
  virtual void enqueue() = 0;

  /// Resets the command buffer encodings.
  ///
  virtual void reset() = 0;

  /// Checks whether the command buffer is pending execution.
  ///
  virtual bool isPending() = 0;

  /// Gets the queue that owns the command buffer.
  ///
  virtual Queue& queue() = 0;
};

/// Queue.
///
class Queue {
 public:
  /// Mask of `CapabilityFlags` bits.
  ///
  using CapabilityMask = uint32_t;

  /// Possible capabilities of a queue.
  ///
  enum CapabilityFlags : uint32_t {
    Graphics = 0x01,
    Compute  = 0x02,
    Transfer = 0x04
  };

  virtual ~Queue();

  /// Creates a new command buffer object.
  ///
  virtual CmdBuffer::Ptr cmdBuffer() = 0;

  /// Submits enqueued command buffers for execution.
  ///
  virtual void submit() = 0;

  /// Gets the capabilities of the queue.
  ///
  virtual CapabilityMask capabilities() const = 0;
};

CG_NS_END

#endif // YF_CG_QUEUE_H
