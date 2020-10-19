//
// cg
// QueueVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cassert>

#include "QueueVK.h"

using namespace CG_NS;
using namespace std;

// ------------------------------------------------------------------------
// CmdBufferVK

CmdBufferVK::CmdBufferVK(QueueVK& queue, VkCommandBuffer handle)
  : _queue(queue), _handle(handle) {
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
  // TODO
  assert(false);
}
Queue& CmdBufferVK::queue() const {
  // TODO
  assert(false);
}

// ------------------------------------------------------------------------
// QueueVK

QueueVK::QueueVK(CapabilityMask capabilities, int32_t family, VkQueue handle)
  : Queue(capabilities), _family(family), _handle(handle) {
  // TODO
  assert(false);
}
QueueVK::~QueueVK() {
  // TODO
  assert(false);
}

void QueueVK::initPool(CmdBufferVK* cmdBuffer) {
  // TODO
  assert(false);
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
