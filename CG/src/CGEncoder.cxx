//
// yf
// CGEncoder.cxx
//
// Copyright (c) 2020 Gustavo C. Viegas.
//

#include "CGEncoder.h"

using namespace YF_NS;
using namespace std;

class CGEncoder::Impl {
  // TODO
};

CGEncoder::CGEncoder() : _impl(make_unique<Impl>()) {}
CGEncoder::~CGEncoder() {}

void CGEncoder::setState(CGGraphState* state) {
  // TODO
}

void CGEncoder::setState(CGCompState* state) {
  // TODO
}

void CGEncoder::setViewport(CGViewport viewport) {
  // TODO
}

void CGEncoder::setScissor(CGScissor scissor) {
  // TODO
}

void CGEncoder::setTarget(CGTarget* target) {
  // TODO
}

void CGEncoder::setDcTable(uint32_t tableIndex, uint32_t allocIndex) {
  // TODO
}

void CGEncoder::setVertexBuffer(uint32_t inputIndex,
                                CGBuffer* buffer,
                                uint64_t offset) {
  // TODO
}

void CGEncoder::setIndexBuffer(CGBuffer* buffer,
                               uint64_t offset,
                               CGIndexType type) {
  // TODO
}

void CGEncoder::draw(uint32_t vertexStart,
                     uint32_t vertexCount,
                     uint32_t baseInstance,
                     uint32_t instanceCount) {
  // TODO
}

void CGEncoder::drawIndexed(uint32_t indexStart,
                            uint32_t vertexCount,
                            int32_t vertexOffset,
                            uint32_t baseInstance,
                            uint32_t instanceCount) {
  // TODO
}

void CGEncoder::dispatch(CGSize3 size) {
  // TODO
}

void CGEncoder::clearColor(uint32_t colorIndex, CGColor value) {
  // TODO
}

void CGEncoder::clearDepth(float value) {
  // TODO
}

void CGEncoder::clearStencil(uint32_t value) {
  // TODO
}
