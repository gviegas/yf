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
  QueueVK(VkQueue handle, int32_t family);
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
  VkCommandBuffer getPriority(VkPipelineStageFlags stageMask,
                              std::function<void (bool)> completionHandler);

  /// Sets a semaphore upon which to wait in the next submission.
  ///
  void waitFor(VkSemaphore semaphore, VkPipelineStageFlags stageMask);

  /// Getters.
  ///
  VkQueue handle() const;
  int32_t family() const;

 private:
  VkCommandPool initPool();
  void deinitPool(VkCommandPool);

  VkQueue handle_ = nullptr;
  int32_t family_ = -1;
  std::unordered_map<CmdBufferVK*, VkCommandPool> pools_{};
  std::unordered_set<CmdBufferVK*> pending_{};

  VkCommandPool poolPrio_ = VK_NULL_HANDLE;
  VkCommandBuffer cmdPrio_ = VK_NULL_HANDLE;
  VkPipelineStageFlags maskPrio_ = 0;
  std::vector<std::function<void (bool)>> callbsPrio_{};
  bool pendPrio_ = false;

  std::vector<VkSemaphore> semaphores_{};
  std::vector<VkPipelineStageFlags> stageMasks_{};
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
