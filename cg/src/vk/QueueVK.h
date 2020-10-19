//
// cg
// QueueVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_QUEUEVK_H
#define YF_CG_QUEUEVK_H

#include <unordered_map>
#include <unordered_set>

#include "yf/cg/Defs.h"
#include "yf/cg/Queue.h"
#include "VK.h"

CG_NS_BEGIN

class QueueVK;

class CmdBufferVK final : public CmdBuffer {
 public:
  explicit CmdBufferVK(QueueVK& queue, VkCommandBuffer handle);
  ~CmdBufferVK();

  Result encode(const Encoder& encoder);
  Result enqueue();
  Result reset();
  bool isPending();
  Queue& queue() const;

 private:
  QueueVK& _queue;
  VkCommandBuffer _handle = nullptr;
  bool _pending = false;
};

class QueueVK final : public Queue {
 public:
  QueueVK(CapabilityMask capabilities, int32_t family, VkQueue handle);
  ~QueueVK();

  CmdBuffer::Ptr makeCmdBuffer();
  Result submit(CompletionFn onCompletion);

  /// Called by `CmdBufferVK` to enqueue itself.
  ///
  void enqueue(CmdBufferVK* cmdBuffer);

  /// Called by `CmdBufferVK` when it is about to be destroyed.
  ///
  void unmake(CmdBufferVK* cmdBuffer) noexcept;

 private:
  void initPool(CmdBufferVK* cmdBuffer);

  int32_t _family = -1;
  VkQueue _handle = nullptr;
  std::unordered_map<CmdBufferVK*, VkCommandPool> _pools{};
  std::unordered_set<CmdBufferVK*> _pending{};
};

CG_NS_END

#endif // YF_CG_QUEUEVK_H
