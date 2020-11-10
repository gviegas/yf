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
  // No command buffer shall outlive its queue
  if (!pools_.empty()) {
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
  if (pending_.empty())
    return;

  auto notifyAndClear = [&] {
    for (auto& cb : pending_)
      cb->didExecute();
    pending_.clear();
  };

  vector<VkCommandBuffer> handles;
  for (const auto& cb : pending_)
    handles.push_back(cb->handle());

  VkSubmitInfo info;
  info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  info.pNext = nullptr;
  info.waitSemaphoreCount = 0;
  info.pWaitSemaphores = nullptr;
  info.pWaitDstStageMask = 0;
  info.commandBufferCount = handles.size();
  info.pCommandBuffers = handles.data();
  info.signalSemaphoreCount = 0;
  info.pSignalSemaphores = nullptr;

  VkResult res;
  res = vkQueueSubmit(handle_, 1, &info, VK_NULL_HANDLE);
  if (res != VK_SUCCESS) {
    notifyAndClear();
    throw DeviceExcept("Queue submission failed");
  }
  res = vkQueueWaitIdle(handle_);
  if (res != VK_SUCCESS) {
    notifyAndClear();
    throw DeviceExcept("Could not wait queue operations to complete");
  }

  notifyAndClear();
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
  auto& encoding = encoder.encoding();
  for (auto& cmd : encoding) {
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
  auto& encoding = encoder.encoding();
  for (auto& cmd : encoding) {
    switch (cmd->cmd) {
    case Cmd::StateCpT:
      // TODO
      assert(false);
    case Cmd::DcTableT:
      // TODO
      assert(false);
    case Cmd::DispatchT:
      // TODO
      assert(false);
    default:
      assert(false);
      abort();
    }
  }
}

void CmdBufferVK::encode(const TfEncoder& encoder) {
  auto& encoding = encoder.encoding();
  for (auto& cmd : encoding) {
    // TODO
    switch (cmd->cmd) {
    default:
      assert(false);
      abort();
    }
  }
}
