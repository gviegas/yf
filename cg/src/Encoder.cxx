//
// cg
// Encoder.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/cg/Encoder.h"
#include "Cmd.h"

using namespace CG_NS;
using namespace std;

// TODO: Filter useless commands & check whether encoding is valid.

class Encoder::Impl {
 public:
  Impl(Type type) : type(type) {}

  const Type type;
  CGEncoding encoding;
};

Encoder::Encoder(Type type) : _impl(make_unique<Impl>(type)) {}
Encoder::~Encoder() {}

Encoder::Type Encoder::type() const {
  return _impl->type;
}

const Encoding& Encoder::encoding() const {
  return _impl->encoding;
}

// -------------------------------------------------------------------------
// Graphics

GrEncoder::GrEncoder() : Encoder(Graphics) {}

void GrEncoder::setState(GrState* state) {
  _impl->encoding
    .push_back(make_unique<StateGrCmd>(state));
}

void GrEncoder::setViewport(Viewport viewport, uint32_t viewportIndex) {
  _impl->encoding
    .push_back(make_unique<ViewportCmd>(viewport, viewportIndex));
}

void GrEncoder::setScissor(Scissor scissor, uint32_t viewportIndex) {
  _impl->encoding
    .push_back(make_unique<ScissorCmd>(scissor, viewportIndex));
}

void GrEncoder::setTarget(Target* target) {
  _impl->encoding
    .push_back(make_unique<TargetCmd>(target));
}

void GrEncoder::setDcTable(uint32_t tableIndex, uint32_t allocIndex) {
  _impl->encoding
    .push_back(make_unique<DcTableCmd>(tableIndex, allocIndex));
}

void GrEncoder::setVertexBuffer(Buffer* buffer,
                                uint64_t offset,
                                uint32_t inputIndex) {
  _impl->encoding
    .push_back(make_unique<VxBufferCmd>(buffer, offset, inputIndex));
}

void GrEncoder::setIndexBuffer(Buffer* buffer,
                               uint64_t offset,
                               IndexType type) {
  _impl->encoding
    .push_back(make_unique<IxBufferCmd>(buffer, offset, type));
}

void GrEncoder::draw(uint32_t vertexStart,
                     uint32_t vertexCount,
                     uint32_t baseInstance,
                     uint32_t instanceCount) {
  _impl->encoding
    .push_back(make_unique<DrawCmd>(vertexStart, vertexCount,
                                    baseInstance, instanceCount));
}

void GrEncoder::drawIndexed(uint32_t indexStart,
                            uint32_t vertexCount,
                            int32_t vertexOffset,
                            uint32_t baseInstance,
                            uint32_t instanceCount) {
  _impl->encoding
    .push_back(make_unique<DrawIxCmd>(indexStart, vertexCount, vertexOffset,
                                      baseInstance, instanceCount));
}

void GrEncoder::clearColor(Color value, uint32_t colorIndex) {
  _impl->encoding
    .push_back(make_unique<ClearClCmd>(value, colorIndex));
}

void GrEncoder::clearDepth(float value) {
  _impl->encoding
    .push_back(make_unique<ClearDpCmd>(value));
}

void GrEncoder::clearStencil(uint32_t value) {
  _impl->encoding
    .push_back(make_unique<ClearScCmd>(value));
}

// -------------------------------------------------------------------------
// Compute

CpEncoder::CpEncoder() : Encoder(Compute) {}

void CpEncoder::setState(CpState* state) {
  _impl->encoding
    .push_back(make_unique<StateCpCmd>(state));
}

void CpEncoder::setDcTable(uint32_t tableIndex, uint32_t allocIndex) {
  _impl->encoding
    .push_back(make_unique<DcTableCmd>(tableIndex, allocIndex));
}

void CpEncoder::dispatch(Size3 size) {
  _impl->encoding
    .push_back(make_unique<DispatchCmd>(size));
}

// -------------------------------------------------------------------------
// Transfer

TfEncoder::TfEncoder() : Encoder(Transfer) {}
