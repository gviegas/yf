//
// yf
// CGEncoder.cxx
//
// Copyright (c) 2020 Gustavo C. Viegas.
//

#include "CGEncoder.h"
#include "CGCmd.h"

using namespace YF_NS;
using namespace std;

// TODO: Filter useless commands & check whether encoding is valid.

class CGEncoder::Impl {
 public:
  CGEncoding encoding;
};

CGEncoder::CGEncoder() : _impl(make_unique<Impl>()) {}
CGEncoder::~CGEncoder() {}

void CGEncoder::setState(CGGraphState* state) {
  _impl->encoding.push_back(make_unique<CGGraphStateCmd>(state));
}

void CGEncoder::setState(CGCompState* state) {
  _impl->encoding.push_back(make_unique<CGCompStateCmd>(state));
}

void CGEncoder::setViewport(CGViewport viewport) {
  _impl->encoding.push_back(make_unique<CGViewportCmd>(viewport));
}

void CGEncoder::setScissor(CGScissor scissor) {
  _impl->encoding.push_back(make_unique<CGScissorCmd>(scissor));
}

void CGEncoder::setTarget(CGTarget* target) {
  _impl->encoding.push_back(make_unique<CGTargetCmd>(target));
}

void CGEncoder::setDcTable(uint32_t tableIndex, uint32_t allocIndex) {
  _impl->encoding.push_back(make_unique<CGDcTableCmd>(tableIndex, allocIndex));
}

void CGEncoder::setVertexBuffer(uint32_t inputIndex,
                                CGBuffer* buffer,
                                uint64_t offset) {

  _impl->encoding.push_back(make_unique<CGVxBufferCmd>(inputIndex,
                                                       buffer,
                                                       offset));
}

void CGEncoder::setIndexBuffer(CGBuffer* buffer,
                               uint64_t offset,
                               CGIndexType type) {

  _impl->encoding.push_back(make_unique<CGIxBufferCmd>(buffer, offset, type));
}

void CGEncoder::draw(uint32_t vertexStart,
                     uint32_t vertexCount,
                     uint32_t baseInstance,
                     uint32_t instanceCount) {

  _impl->encoding.push_back(make_unique<CGDrawCmd>(vertexStart,
                                                   vertexCount,
                                                   baseInstance,
                                                   instanceCount));
}

void CGEncoder::drawIndexed(uint32_t indexStart,
                            uint32_t vertexCount,
                            int32_t vertexOffset,
                            uint32_t baseInstance,
                            uint32_t instanceCount) {

  _impl->encoding.push_back(make_unique<CGDrawIxCmd>(indexStart,
                                                     vertexCount,
                                                     vertexOffset,
                                                     baseInstance,
                                                     instanceCount));
}

void CGEncoder::dispatch(CGSize3 size) {
  _impl->encoding.push_back(make_unique<CGDispatchCmd>(size));
}

void CGEncoder::clearColor(uint32_t colorIndex, CGColor value) {
  _impl->encoding.push_back(make_unique<CGClearColCmd>(colorIndex, value));
}

void CGEncoder::clearDepth(float value) {
  _impl->encoding.push_back(make_unique<CGClearDepCmd>(value));
}

void CGEncoder::clearStencil(uint32_t value) {
  _impl->encoding.push_back(make_unique<CGClearStenCmd>(value));
}

const CGEncoding& CGEncoder::encoding() const {
  return _impl->encoding;
}
