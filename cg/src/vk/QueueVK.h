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

  void encode(const Encoder& encoder);
  void enqueue();
  void reset();
  bool isPending();
  Queue& queue() const;

  /// The command buffer handle.
  ///
  VkCommandBuffer handle() const;

  /// Called by `QueueVK` when execution of this command buffer completes.
  ///
  void didExecute();

 private:
  QueueVK& _queue;
  VkCommandBuffer _handle = nullptr;
  bool _pending = false;
};

class QueueVK final : public Queue {
 public:
  QueueVK(int32_t family, VkQueue handle);
  ~QueueVK();

  CmdBuffer::Ptr makeCmdBuffer();
  void submit();

  /// Called by `CmdBufferVK` to enqueue itself.
  ///
  void enqueue(CmdBufferVK* cmdBuffer);

  /// Called by `CmdBufferVK` when it is about to be destroyed.
  ///
  void unmake(CmdBufferVK* cmdBuffer) noexcept;

  /// Called by `DeviceVK` to signal that procedures can now be set.
  ///
  void setProcs(VkDevice device, uint32_t version);

 private:
  VkCommandPool initPool();
  void deinitPool(VkCommandPool pool);

  int32_t _family = -1;
  VkQueue _handle = nullptr;
  std::unordered_map<CmdBufferVK*, VkCommandPool> _pools{};
  std::unordered_set<CmdBufferVK*> _pending{};
};

CG_NS_END

#endif // YF_CG_QUEUEVK_H
