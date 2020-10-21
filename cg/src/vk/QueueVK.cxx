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
  //assert(false);
}
CmdBufferVK::~CmdBufferVK() {
  // TODO
  //assert(false);
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
PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = nullptr;
// v1.1
PFN_vkTrimCommandPool vkTrimCommandPool = nullptr;

INTERNAL_NS_END

void QueueVK::setProcs(VkDevice device, uint32_t version) {
  vkCreateCommandPool = CG_DEVPROCVK_RVAL(device, vkCreateCommandPool);
  vkResetCommandPool = CG_DEVPROCVK_RVAL(device, vkResetCommandPool);
  vkDestroyCommandPool = CG_DEVPROCVK_RVAL(device, vkDestroyCommandPool);
  vkAllocateCommandBuffers =
  CG_DEVPROCVK_RVAL(device, vkAllocateCommandBuffers);

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

Result QueueVK::submit(CompletionFn onCompletion) {
  // TODO
  assert(false);
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
