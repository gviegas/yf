//
// CG
// QueueVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_QUEUEVK_H
#define YF_CG_QUEUEVK_H

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <vector>

#include "Defs.h"
#include "Queue.h"
#include "VK.h"

CG_NS_BEGIN

class CmdBufferVK;

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

  /// Gets a command buffer handle that executes before the next batch.
  ///
  VkCommandBuffer getPriority(std::function<void (bool)> completionHandler);

 private:
  VkCommandPool initPool();
  void deinitPool(VkCommandPool);

  int32_t family_ = -1;
  VkQueue handle_ = nullptr;
  std::unordered_map<CmdBufferVK*, VkCommandPool> pools_{};
  std::unordered_set<CmdBufferVK*> pending_{};

  VkCommandPool poolPrio_ = VK_NULL_HANDLE;
  VkCommandBuffer cmdPrio_ = VK_NULL_HANDLE;
  std::vector<std::function<void (bool)>> callbsPrio_{};
  bool pendPrio_ = false;
};

class GrEncoder;
class CpEncoder;
class TfEncoder;

class CmdBufferVK final : public CmdBuffer {
 public:
  explicit CmdBufferVK(QueueVK& queue, VkCommandBuffer handle);
  ~CmdBufferVK();

  void encode(const Encoder& encoder);
  void enqueue();
  void reset();
  bool isPending();
  Queue& queue() const;

  /// Getter.
  ///
  VkCommandBuffer handle() const;

  /// Called by `QueueVK` when execution of this command buffer completes.
  ///
  void didExecute();

 private:
  void encode(const GrEncoder&);
  void encode(const CpEncoder&);
  void encode(const TfEncoder&);

  QueueVK& queue_;
  VkCommandBuffer handle_ = nullptr;
  bool pending_ = false;
  bool begun_ = false;
};

CG_NS_END

#endif // YF_CG_QUEUEVK_H
