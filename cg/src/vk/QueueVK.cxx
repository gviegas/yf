//
// cg
// QueueVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cassert>

#include "QueueVK.h"
#include "DeviceVK.h"
#include "Cmd.h"
#include "yf/cg/Encoder.h"

using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

/// VK procedures used by `QueueVK` and `CmdBufferVK`.
///
PFN_vkCreateCommandPool vkCreateCommandPool = nullptr;
PFN_vkResetCommandPool vkResetCommandPool = nullptr;
PFN_vkDestroyCommandPool vkDestroyCommandPool = nullptr;
PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = nullptr;
PFN_vkResetCommandBuffer vkResetCommandBuffer = nullptr;
PFN_vkBeginCommandBuffer vkBeginCommandBuffer = nullptr;
PFN_vkEndCommandBuffer vkEndCommandBuffer = nullptr;
PFN_vkQueueSubmit vkQueueSubmit = nullptr;
PFN_vkQueueWaitIdle vkQueueWaitIdle = nullptr;
// v1.1
PFN_vkTrimCommandPool vkTrimCommandPool = nullptr;

INTERNAL_NS_END

// ------------------------------------------------------------------------
// QueueVK

void QueueVK::setProcs(VkDevice device, uint32_t version) {
  vkCreateCommandPool = CG_DEVPROCVK_RVAL(device, vkCreateCommandPool);
  vkResetCommandPool = CG_DEVPROCVK_RVAL(device, vkResetCommandPool);
  vkDestroyCommandPool = CG_DEVPROCVK_RVAL(device, vkDestroyCommandPool);

  vkAllocateCommandBuffers = CG_DEVPROCVK_RVAL(device, vkAllocateCommandBuffers);
  vkResetCommandBuffer = CG_DEVPROCVK_RVAL(device, vkResetCommandBuffer);

  vkBeginCommandBuffer = CG_DEVPROCVK_RVAL(device, vkBeginCommandBuffer);
  vkEndCommandBuffer = CG_DEVPROCVK_RVAL(device, vkEndCommandBuffer);

  vkQueueSubmit = CG_DEVPROCVK_RVAL(device, vkQueueSubmit);
  vkQueueWaitIdle = CG_DEVPROCVK_RVAL(device, vkQueueWaitIdle);

  if (version >= VK_MAKE_VERSION(1, 1, 0))
    vkTrimCommandPool = CG_DEVPROCVK_RVAL(device, vkTrimCommandPool);
  else
    vkTrimCommandPool = nullptr;
}

QueueVK::QueueVK(int32_t family, VkQueue handle)
  : Queue(Graphics|Compute|Transfer), _family(family), _handle(handle) {

  assert(family > -1);
  assert(handle != nullptr);

  // XXX: `DeviceVK` not fully constructed yet
}

QueueVK::~QueueVK() {
  // No command buffer shall outlive its queue
  if (!_pools.empty()) {
    assert(false);
    abort();
  }
}

VkCommandPool QueueVK::initPool() {
  VkCommandPoolCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  info.queueFamilyIndex = _family;

  VkCommandPool handle;
  auto res = vkCreateCommandPool(DeviceVK::get().device(), &info, nullptr,
                                 &handle);
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Could not create command pool");

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
    // TODO
    throw runtime_error("Could not allocate command buffer");
  }

  auto it = _pools.emplace(new CmdBufferVK(*this, handle), pool).first;
  return CmdBuffer::Ptr(it->first);
}

void QueueVK::submit() {
  if (_pending.empty())
    return;

  auto notifyAndClear = [&] {
    for (auto& cb : _pending)
      cb->didExecute();
    _pending.clear();
  };

  vector<VkCommandBuffer> handles;
  for (const auto& cb : _pending)
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
  res = vkQueueSubmit(_handle, 1, &info, VK_NULL_HANDLE);
  if (res != VK_SUCCESS) {
    notifyAndClear();
    // TODO
    throw runtime_error("Queue submission failed");
  }
  res = vkQueueWaitIdle(_handle);
  if (res != VK_SUCCESS) {
    notifyAndClear();
    // TODO
    throw runtime_error("Could not wait queue operations to complete");
  }

  notifyAndClear();
}

void QueueVK::enqueue(CmdBufferVK* cmdBuffer) {
  assert(_pending.find(cmdBuffer) == _pending.end());

  // TODO: lock
  _pending.insert(cmdBuffer);
}

void QueueVK::unmake(CmdBufferVK* cmdBuffer) noexcept {
  assert(_pools.find(cmdBuffer) != _pools.end());

  if (cmdBuffer->isPending()) {
    // TODO: gate command buffer destruction
    assert(false);
    abort();
  }

  auto it = _pools.find(cmdBuffer);
  vkDestroyCommandPool(DeviceVK::get().device(), it->second, nullptr);
  _pools.erase(it);
}

// ------------------------------------------------------------------------
// CmdBufferVK

CmdBufferVK::CmdBufferVK(QueueVK& queue, VkCommandBuffer handle)
  : _queue(queue), _handle(handle), _pending(false), _begun(false) {

  assert(handle != nullptr);
}

CmdBufferVK::~CmdBufferVK() {
  _queue.unmake(this);
}

void CmdBufferVK::encode(const Encoder& encoder) {
  if (_pending)
    // TODO
    throw runtime_error("Attempt to encode a pending command buffer");

  if (!_begun) {
    VkCommandBufferBeginInfo info;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    // TODO: consider reusing the command buffer instead
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    info.pInheritanceInfo = nullptr;

    auto res = vkBeginCommandBuffer(_handle, &info);
    if (res != VK_SUCCESS)
      // TODO
      throw runtime_error("Could not set command buffer for encoding");

    _begun = true;
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
  if (_pending)
    // TODO
    throw runtime_error("Attempt to enqueue a pending command buffer");

  if (!_begun)
    // TODO
    throw runtime_error("Attempt to enqueue an empty command buffer");

  _begun = false;

  auto res = vkEndCommandBuffer(_handle);
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Invalid command buffer encoding(s)");

  _pending = true;
  _queue.enqueue(this);
}

void CmdBufferVK::reset() {
  if (_pending)
    // TODO
    throw runtime_error("Attempt to reset a pending command buffer");

  vkResetCommandBuffer(_handle, 0);
}

bool CmdBufferVK::isPending() {
  return _pending;
}

Queue& CmdBufferVK::queue() const {
  return _queue;
}

VkCommandBuffer CmdBufferVK::handle() const {
  return _handle;
}

void CmdBufferVK::didExecute() {
  assert(_pending);

  _pending = false;
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
