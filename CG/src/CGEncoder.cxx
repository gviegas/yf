//
// yf
// CGEncoder.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "CGEncoder.h"
#include "CGCmd.h"

using namespace YF_NS;
using namespace std;

// TODO: Filter useless commands & check whether encoding is valid.

class CGEncoder::Impl {
 public:
  Impl(Type type) : type(type) {}

  const Type type;
  CGEncoding encoding;
};

CGEncoder::CGEncoder(Type type) : _impl(make_unique<Impl>(type)) {}
CGEncoder::~CGEncoder() {}

CGEncoder::Type CGEncoder::type() const {
  return _impl->type;
}

const CGEncoding& CGEncoder::encoding() const {
  return _impl->encoding;
}

// -------------------------------------------------------------------------
// Graphics

CGGrEncoder::CGGrEncoder() : CGEncoder(Graphics) {}

void CGGrEncoder::setState(CGGrState* state) {
  _impl->encoding.push_back(make_unique<CGStateGrCmd>(state));
}

void CGGrEncoder::setViewport(CGViewport viewport, uint32_t viewportIndex) {
  _impl->encoding.push_back(make_unique<CGViewportCmd>(viewport,
                                                       viewportIndex));
}

void CGGrEncoder::setScissor(CGScissor scissor, uint32_t viewportIndex) {
  _impl->encoding.push_back(make_unique<CGScissorCmd>(scissor, viewportIndex));
}

void CGGrEncoder::setTarget(CGTarget* target) {
  _impl->encoding.push_back(make_unique<CGTargetCmd>(target));
}

void CGGrEncoder::setDcTable(uint32_t tableIndex, uint32_t allocIndex) {
  _impl->encoding.push_back(make_unique<CGDcTableCmd>(tableIndex, allocIndex));
}

void CGGrEncoder::setVertexBuffer(CGBuffer* buffer,
                                  uint64_t offset,
                                  uint32_t inputIndex) {

  _impl->encoding.push_back(make_unique<CGVxBufferCmd>(buffer,
                                                       offset,
                                                       inputIndex));
}

void CGGrEncoder::setIndexBuffer(CGBuffer* buffer,
                                 uint64_t offset,
                                 CGIndexType type) {

  _impl->encoding.push_back(make_unique<CGIxBufferCmd>(buffer, offset, type));
}

void CGGrEncoder::draw(uint32_t vertexStart,
                       uint32_t vertexCount,
                       uint32_t baseInstance,
                       uint32_t instanceCount) {

  _impl->encoding.push_back(make_unique<CGDrawCmd>(vertexStart,
                                                   vertexCount,
                                                   baseInstance,
                                                   instanceCount));
}

void CGGrEncoder::drawIndexed(uint32_t indexStart,
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

void CGGrEncoder::clearColor(CGColor value, uint32_t colorIndex) {
  _impl->encoding.push_back(make_unique<CGClearClCmd>(value, colorIndex));
}

void CGGrEncoder::clearDepth(float value) {
  _impl->encoding.push_back(make_unique<CGClearDpCmd>(value));
}

void CGGrEncoder::clearStencil(uint32_t value) {
  _impl->encoding.push_back(make_unique<CGClearScCmd>(value));
}

// -------------------------------------------------------------------------
// Compute

CGCpEncoder::CGCpEncoder() : CGEncoder(Compute) {}

void CGCpEncoder::setState(CGCpState* state) {
  _impl->encoding.push_back(make_unique<CGStateCpCmd>(state));
}

void CGCpEncoder::setDcTable(uint32_t tableIndex, uint32_t allocIndex) {
  _impl->encoding.push_back(make_unique<CGDcTableCmd>(tableIndex, allocIndex));
}

void CGCpEncoder::dispatch(CGSize3 size) {
  _impl->encoding.push_back(make_unique<CGDispatchCmd>(size));
}

// -------------------------------------------------------------------------
// Transfer

CGTfEncoder::CGTfEncoder() : CGEncoder(Transfer) {}
