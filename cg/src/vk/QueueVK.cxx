//
// cg
// QueueVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cassert>

#include "QueueVK.h"
#include "DeviceVK.h"

using namespace CG_NS;
using namespace std;

// ------------------------------------------------------------------------
// CmdBufferVK

CmdBufferVK::CmdBufferVK(QueueVK& queue, VkCommandBuffer handle)
  : _queue(queue), _handle(handle), _pending(false) {
  // TODO
  assert(false);
}
CmdBufferVK::~CmdBufferVK() {
  // TODO
  assert(false);
}

Result CmdBufferVK::encode(const Encoder& encoder) {
  // TODO
  assert(false);
}
Result CmdBufferVK::enqueue() {
  // TODO
  assert(false);
}
Result CmdBufferVK::reset() {
  // TODO
  assert(false);
}
bool CmdBufferVK::isPending() {
  return _pending;
}
Queue& CmdBufferVK::queue() const {
  return _queue;
}

// ------------------------------------------------------------------------
// QueueVK

INTERNAL_NS_BEGIN

/// VK procedures used by `QueueVK`.
///
PFN_vkCreateCommandPool vkCreateCommandPool = nullptr;
PFN_vkResetCommandPool vkResetCommandPool = nullptr;
PFN_vkDestroyCommandPool vkDestroyCommandPool = nullptr;
// v1.1
PFN_vkTrimCommandPool vkTrimCommandPool = nullptr;

/// Sets VK procedures used by `QueueVK`.
///
void setProcs(bool replacing = false) {
  static bool done = false;
  if (!replacing && done)
    return;

  auto dev = DeviceVK::get().device();
  auto devVers = DeviceVK::get().devVersion();

  vkCreateCommandPool = CG_DEVPROCVK_RVAL(dev, vkCreateCommandPool);
  vkResetCommandPool = CG_DEVPROCVK_RVAL(dev, vkResetCommandPool);
  vkDestroyCommandPool = CG_DEVPROCVK_RVAL(dev, vkDestroyCommandPool);

  if (devVers >= VK_MAKE_VERSION(1, 1, 0))
    vkTrimCommandPool = CG_DEVPROCVK_RVAL(dev, vkTrimCommandPool);
  else
    vkTrimCommandPool = nullptr;

  done = true;
}

INTERNAL_NS_END

QueueVK::QueueVK(CapabilityMask capabilities, int32_t family, VkQueue handle)
  : Queue(capabilities), _family(family), _handle(handle) {}

QueueVK::~QueueVK() {
  // TODO
  assert(false);
}

void QueueVK::initPool(CmdBufferVK* cmdBuffer) {
  if (_pools.find(cmdBuffer) != _pools.end())
    deinitPool(cmdBuffer);

  auto it = _pools.emplace(cmdBuffer, /*VK_NULL_HANDLE*/ nullptr).first;

  VkCommandPoolCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  info.queueFamilyIndex = _family;

  auto res = vkCreateCommandPool(DeviceVK::get().device(), &info, nullptr,
                                 &it->second);
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Could not create command pool");
}

void QueueVK::deinitPool(CmdBufferVK* cmdBuffer) {
  auto it = _pools.find(cmdBuffer);
  if (it == _pools.end())
    return;

  if (cmdBuffer != nullptr && cmdBuffer->isPending())
    // TODO
    throw runtime_error("Attempt to deinitialize a command pool"
                        " from a pending command buffer");

  vkDestroyCommandPool(DeviceVK::get().device(), it->second, nullptr);
  _pools.erase(it);
}

CmdBuffer::Ptr QueueVK::makeCmdBuffer() {
  // TODO
  assert(false);
}

Result QueueVK::submit(CompletionFn onCompletion) {
  // TODO
  assert(false);
}

void QueueVK::enqueue(CmdBufferVK* cmdBuffer) {
  // TODO
  assert(false);
}

void QueueVK::unmake(CmdBufferVK* cmdBuffer) noexcept {
  // TODO
  assert(false);
}
