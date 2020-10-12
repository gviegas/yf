//
// yf
// CGQueue.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_QUEUE_H
#define YF_CG_QUEUE_H

#include <cstdint>
#include <functional>
#include <memory>

#include "YFDefs.h"
#include "CGResult.h"

YF_NS_BEGIN

class CGQueue;
class CGEncoder;

class CGCmdBuffer {
 public:
  CGCmdBuffer() = default;
  virtual ~CGCmdBuffer();

  /// Encodes the command buffer with the contents of an encoder object.
  ///
  virtual CGResult encode(const CGEncoder& encoder) = 0;

  /// Enqueues the command buffer for execution.
  ///
  virtual CGResult enqueue() = 0;

  /// Resets the command buffer encodings.
  ///
  virtual CGResult reset() = 0;

  /// Checks whether the command buffer is ready for use.
  ///
  virtual bool isReady() = 0;

  /// The queue that owns the command buffer.
  ///
  virtual const CGQueue& queue() const = 0;
};

class CGQueue {
 public:
  using CapabilityMask = uint32_t;
  enum CapabilityFlags : uint32_t {
    Graphics = 0x01,
    Compute  = 0x02,
    Transfer = 0x04
  };

  explicit CGQueue(CapabilityMask capabilities);
  virtual ~CGQueue();

  /// Makes a new command buffer object.
  ///
  using CmdBufferRes = CGResultPair<std::unique_ptr<CGCmdBuffer>>;
  virtual CmdBufferRes makeCmdBuffer() = 0;

  /// Submits enqueued command buffers for execution.
  ///
  using CompletionFn = std::function<CGResult ()>;
  virtual CGResult submit(CompletionFn onCompletion) = 0;

  /// The capabilities of the queue.
  ///
  const CapabilityMask capabilities;
};

YF_NS_END

#endif // YF_CG_QUEUE_H
