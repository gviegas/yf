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
  _impl->encoding.push_back(make_unique<CGStateGpCmd>(state));
}

void CGEncoder::setState(CGCompState* state) {
  _impl->encoding.push_back(make_unique<CGStateCmCmd>(state));
}

void CGEncoder::setViewport(CGViewport viewport, uint32_t viewportIndex) {
  _impl->encoding.push_back(make_unique<CGViewportCmd>(viewport,
                                                       viewportIndex));
}

void CGEncoder::setScissor(CGScissor scissor, uint32_t viewportIndex) {
  _impl->encoding.push_back(make_unique<CGScissorCmd>(scissor, viewportIndex));
}

void CGEncoder::setTarget(CGTarget* target) {
  _impl->encoding.push_back(make_unique<CGTargetCmd>(target));
}

void CGEncoder::setDcTable(uint32_t tableIndex, uint32_t allocIndex) {
  _impl->encoding.push_back(make_unique<CGDcTableCmd>(tableIndex, allocIndex));
}

void CGEncoder::setVertexBuffer(CGBuffer* buffer,
                                uint64_t offset,
                                uint32_t inputIndex) {

  _impl->encoding.push_back(make_unique<CGVxBufferCmd>(buffer,
                                                       offset,
                                                       inputIndex));
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

void CGEncoder::clearColor(CGColor value, uint32_t colorIndex) {
  _impl->encoding.push_back(make_unique<CGClearClCmd>(value, colorIndex));
}

void CGEncoder::clearDepth(float value) {
  _impl->encoding.push_back(make_unique<CGClearDpCmd>(value));
}

void CGEncoder::clearStencil(uint32_t value) {
  _impl->encoding.push_back(make_unique<CGClearScCmd>(value));
}

const CGEncoding& CGEncoder::encoding() const {
  return _impl->encoding;
}
