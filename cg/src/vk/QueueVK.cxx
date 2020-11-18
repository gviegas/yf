//
// CG
// QueueVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cassert>
#include <stdexcept>

#include "QueueVK.h"
#include "DeviceVK.h"
#include "DcTableVK.h"
#include "StateVK.h"
#include "Cmd.h"
#include "Encoder.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

// ------------------------------------------------------------------------
// QueueVK

QueueVK::QueueVK(int32_t family, VkQueue handle)
  : Queue(Graphics|Compute|Transfer), family_(family), handle_(handle) {

  assert(family > -1);
  assert(handle != nullptr);

  // XXX: `DeviceVK` not fully constructed yet
}

QueueVK::~QueueVK() {
  deinitPool(poolPrio_);
  if (!pools_.empty()) {
    // XXX: no command buffer shall outlive its queue
    assert(false);
    abort();
  }
}

VkCommandPool QueueVK::initPool() {
  VkCommandPoolCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  info.queueFamilyIndex = family_;

  VkCommandPool handle;
  auto res = vkCreateCommandPool(DeviceVK::get().device(), &info, nullptr,
                                 &handle);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create command pool");

  return handle;
}

void QueueVK::deinitPool(VkCommandPool pool) {
  vkDestroyCommandPool(DeviceVK::get().device(), pool, nullptr);
}

CmdBuffer::Ptr QueueVK::makeCmdBuffer() {
  auto pool = initPool();

  VkCommandBufferAllocateInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.pNext = nullptr;
  info.commandPool = pool;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  info.commandBufferCount = 1;

  VkCommandBuffer handle;
  auto res = vkAllocateCommandBuffers(DeviceVK::get().device(), &info, &handle);
  if (res != VK_SUCCESS) {
    deinitPool(pool);
    throw DeviceExcept("Could not allocate command buffer");
  }

  auto it = pools_.emplace(new CmdBufferVK(*this, handle), pool).first;
  return CmdBuffer::Ptr(it->first);
}

void QueueVK::submit() {
  auto dev = DeviceVK::get().device();
  VkSemaphore sem = VK_NULL_HANDLE;
  VkResult res;

  auto notifyAndClear = [&](bool result) {
    for (auto& fn : callbsPrio_)
      fn(result);
    callbsPrio_.clear();
    pendPrio_ = false;

    for (auto& cb : pending_)
      cb->didExecute();
    pending_.clear();

    vkDestroySemaphore(dev, sem, nullptr);
  };

  if (pendPrio_) {
    res = vkEndCommandBuffer(cmdPrio_);
    if (res != VK_SUCCESS) {
      notifyAndClear(false);
      throw DeviceExcept("Could not end priority command buffer");
    }
  } else if (pending_.empty()) {
    // Nothing to do
    return;
  }

  // Set submission info
  VkSubmitInfo infos[2];
  uint32_t infoN = 0;
  vector<VkCommandBuffer> handles;
  uint32_t handleN = 0;

  if (pendPrio_) {
    handles.push_back(cmdPrio_);

    infos[infoN].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    infos[infoN].pNext = nullptr;
    infos[infoN].waitSemaphoreCount = 0;
    infos[infoN].pWaitSemaphores = nullptr;
    infos[infoN].pWaitDstStageMask = nullptr;
    infos[infoN].commandBufferCount = 1;
    infos[infoN].pCommandBuffers = handles.data();
    infos[infoN].signalSemaphoreCount = 0;
    infos[infoN].pSignalSemaphores = nullptr;

    ++infoN;
    ++handleN;
  }

  if (!pending_.empty()) {
    for (const auto& cb : pending_)
      handles.push_back(cb->handle());

    infos[infoN].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    infos[infoN].pNext = nullptr;
    infos[infoN].waitSemaphoreCount = 0;
    infos[infoN].pWaitSemaphores = nullptr;
    infos[infoN].pWaitDstStageMask = nullptr;
    infos[infoN].commandBufferCount = pending_.size();
    infos[infoN].pCommandBuffers = handles.data()+handleN;
    infos[infoN].signalSemaphoreCount = 0;
    infos[infoN].pSignalSemaphores = nullptr;

    ++infoN;
  }

  // Sync. setup
  VkPipelineStageFlags waitDst = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
  if (infoN == 2) {
    VkSemaphoreCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    res = vkCreateSemaphore(dev, &info, nullptr, &sem);
    if (res != VK_SUCCESS) {
      notifyAndClear(false);
      throw DeviceExcept("Could not create semaphore for queue submission");
    }

    infos[0].signalSemaphoreCount = 1;
    infos[0].pSignalSemaphores = &sem;

    infos[1].waitSemaphoreCount = 1;
    infos[1].pWaitSemaphores = &sem;
    infos[1].pWaitDstStageMask = &waitDst;
  }

  // Submit and wait completion
  res = vkQueueSubmit(handle_, infoN, infos, VK_NULL_HANDLE);
  if (res != VK_SUCCESS) {
    notifyAndClear(false);
    throw DeviceExcept("Queue submission failed");
  }

  res = vkQueueWaitIdle(handle_);
  if (res != VK_SUCCESS) {
    notifyAndClear(false);
    throw DeviceExcept("Could not wait for queue operations to complete");
  }

  notifyAndClear(true);
}

void QueueVK::enqueue(CmdBufferVK* cmdBuffer) {
  assert(pending_.find(cmdBuffer) == pending_.end());

  // TODO: lock
  pending_.insert(cmdBuffer);
}

void QueueVK::unmake(CmdBufferVK* cmdBuffer) noexcept {
  assert(pools_.find(cmdBuffer) != pools_.end());

  if (cmdBuffer->isPending()) {
    // TODO: gate command buffer destruction
    assert(false);
    abort();
  }

  auto it = pools_.find(cmdBuffer);
  vkDestroyCommandPool(DeviceVK::get().device(), it->second, nullptr);
  pools_.erase(it);
}

VkCommandBuffer QueueVK::getPriority(function<void (bool)> completionHandler) {
  if (pendPrio_) {
    callbsPrio_.push_back(completionHandler);
    return cmdPrio_;
  }

  VkResult res;

  if (!cmdPrio_) {
    poolPrio_ = initPool();

    VkCommandBufferAllocateInfo info;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = poolPrio_;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = 1;

    res = vkAllocateCommandBuffers(DeviceVK::get().device(), &info, &cmdPrio_);
    if (res != VK_SUCCESS) {
      deinitPool(poolPrio_);
      poolPrio_ = VK_NULL_HANDLE;
      throw DeviceExcept("Could not allocate command buffer");
    }
  }

  VkCommandBufferBeginInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  info.pNext = nullptr;
  info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  info.pInheritanceInfo = nullptr;

  res = vkBeginCommandBuffer(cmdPrio_, &info);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not begin command buffer");

  callbsPrio_.push_back(completionHandler);
  pendPrio_ = true;
  return cmdPrio_;
}

// ------------------------------------------------------------------------
// CmdBufferVK

CmdBufferVK::CmdBufferVK(QueueVK& queue, VkCommandBuffer handle)
  : queue_(queue), handle_(handle), pending_(false), begun_(false) {

  assert(handle != nullptr);
}

CmdBufferVK::~CmdBufferVK() {
  queue_.unmake(this);
}

void CmdBufferVK::encode(const Encoder& encoder) {
  if (pending_)
    throw runtime_error("Attempt to encode a pending command buffer");

  if (!begun_) {
    VkCommandBufferBeginInfo info;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    // TODO: consider reusing the command buffer instead
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    info.pInheritanceInfo = nullptr;

    auto res = vkBeginCommandBuffer(handle_, &info);
    if (res != VK_SUCCESS)
      throw DeviceExcept("Could not set command buffer for encoding");

    begun_ = true;
  }

  // TODO: handle exceptions that might be throw due to encoding failure
  switch (encoder.type()) {
  case Encoder::Graphics:
    encode(static_cast<const GrEncoder&>(encoder));
    break;
  case Encoder::Compute:
    encode(static_cast<const CpEncoder&>(encoder));
    break;
  case Encoder::Transfer:
    encode(static_cast<const TfEncoder&>(encoder));
    break;
  }
}

void CmdBufferVK::enqueue() {
  if (pending_)
    throw runtime_error("Attempt to enqueue a pending command buffer");

  if (!begun_)
    throw runtime_error("Attempt to enqueue an empty command buffer");

  begun_ = false;

  auto res = vkEndCommandBuffer(handle_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Invalid command buffer encoding(s)");

  pending_ = true;
  queue_.enqueue(this);
}

void CmdBufferVK::reset() {
  if (pending_)
    throw runtime_error("Attempt to reset a pending command buffer");

  vkResetCommandBuffer(handle_, 0);
}

bool CmdBufferVK::isPending() {
  return pending_;
}

Queue& CmdBufferVK::queue() const {
  return queue_;
}

VkCommandBuffer CmdBufferVK::handle() const {
  return handle_;
}

void CmdBufferVK::didExecute() {
  assert(pending_);

  pending_ = false;
}

void CmdBufferVK::encode(const GrEncoder& encoder) {
  for (const auto& cmd : encoder.encoding()) {
    switch (cmd->cmd) {
    case Cmd::StateGrT:
      // TODO
      assert(false);
    case Cmd::ViewportT:
      // TODO
      assert(false);
    case Cmd::ScissorT:
      // TODO
      assert(false);
    case Cmd::TargetT:
      // TODO
      assert(false);
    case Cmd::DcTableT:
      // TODO
      assert(false);
    case Cmd::VxBufferT:
      // TODO
      assert(false);
    case Cmd::IxBufferT:
      // TODO
      assert(false);
    case Cmd::DrawT:
      // TODO
      assert(false);
    case Cmd::DrawIxT:
      // TODO
      assert(false);
    case Cmd::ClearClT:
      // TODO
      assert(false);
    case Cmd::ClearDpT:
      // TODO
      assert(false);
    case Cmd::ClearScT:
      // TODO
      assert(false);
    default:
      assert(false);
      abort();
    }
  }
}

void CmdBufferVK::encode(const CpEncoder& encoder) {
  CpStateVK* cst = nullptr;
  vector<const DcTableCmd*> dtbs;

  // Set cp state
  auto setState = [&](const StateCpCmd* sub) {
    cst = static_cast<CpStateVK*>(sub->state);
    auto pl = cst->pipeline();
    vkCmdBindPipeline(handle_, VK_PIPELINE_BIND_POINT_COMPUTE, pl);
  };

  // Set dc table
  auto setDcTable = [&](const DcTableCmd* sub) {
    dtbs.push_back(sub);
  };

  // Dispatch
  auto dispatch = [&](const DispatchCmd* sub) {
    if (!cst)
      throw invalid_argument("dispatch() requires a state to be set");

    if (!dtbs.empty()) {
      auto plLay = cst->plLayout();

      for (const auto& d : dtbs) {
        auto i = d->tableIndex;
        auto j = d->allocIndex;

        if (i >= cst->config_.dcTables.size() ||
            j >= cst->config_.dcTables[i]->allocations())
          throw invalid_argument("setDcTable() index out of range");

        auto ds = static_cast<DcTableVK*>(cst->config_.dcTables[i])->ds(j);
        vkCmdBindDescriptorSets(handle_, VK_PIPELINE_BIND_POINT_COMPUTE,
                                plLay, i, 1, &ds, 0, nullptr);
      }

      dtbs.clear();
    }

    // TODO: check limits
    vkCmdDispatch(handle_, sub->size.width, sub->size.height, sub->size.depth);
  };

  for (const auto& cmd : encoder.encoding()) {
    switch (cmd->cmd) {
    case Cmd::StateCpT:
      setState(static_cast<StateCpCmd*>(cmd.get()));
      break;
    case Cmd::DcTableT:
      setDcTable(static_cast<DcTableCmd*>(cmd.get()));
      break;
    case Cmd::DispatchT:
      dispatch(static_cast<DispatchCmd*>(cmd.get()));
      break;
    default:
      assert(false);
      abort();
    }
  }
}

void CmdBufferVK::encode(const TfEncoder& encoder) {
  for (const auto& cmd : encoder.encoding()) {
    // TODO
    switch (cmd->cmd) {
    default:
      assert(false);
      abort();
    }
  }
}
