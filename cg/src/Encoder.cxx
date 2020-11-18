//
// CG
// Encoder.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Encoder.h"
#include "Cmd.h"

using namespace CG_NS;
using namespace std;

// TODO: Filter useless commands & check whether encoding is valid.

class Encoder::Impl {
 public:
  Impl(Type type) : type_(type) { }

  const Type type_;
  Encoding encoding_;
};

Encoder::Encoder(Type type) : impl_(make_unique<Impl>(type)) { }
Encoder::~Encoder() { }

Encoder::Type Encoder::type() const {
  return impl_->type_;
}

const Encoding& Encoder::encoding() const {
  return impl_->encoding_;
}

// -------------------------------------------------------------------------
// GrEncoder

GrEncoder::GrEncoder() : Encoder(Graphics) { }

void GrEncoder::setState(GrState* state) {
  impl_->encoding_
    .push_back(make_unique<StateGrCmd>(state));
}

void GrEncoder::setViewport(Viewport viewport, uint32_t viewportIndex) {
  impl_->encoding_
    .push_back(make_unique<ViewportCmd>(viewport, viewportIndex));
}

void GrEncoder::setScissor(Scissor scissor, uint32_t viewportIndex) {
  impl_->encoding_
    .push_back(make_unique<ScissorCmd>(scissor, viewportIndex));
}

void GrEncoder::setTarget(Target* target) {
  impl_->encoding_
    .push_back(make_unique<TargetCmd>(target));
}

void GrEncoder::setDcTable(uint32_t tableIndex, uint32_t allocIndex) {
  impl_->encoding_
    .push_back(make_unique<DcTableCmd>(tableIndex, allocIndex));
}

void GrEncoder::setVertexBuffer(Buffer* buffer,
                                uint64_t offset,
                                uint32_t inputIndex) {
  impl_->encoding_
    .push_back(make_unique<VxBufferCmd>(buffer, offset, inputIndex));
}

void GrEncoder::setIndexBuffer(Buffer* buffer,
                               uint64_t offset,
                               IndexType type) {
  impl_->encoding_
    .push_back(make_unique<IxBufferCmd>(buffer, offset, type));
}

void GrEncoder::draw(uint32_t vertexStart,
                     uint32_t vertexCount,
                     uint32_t baseInstance,
                     uint32_t instanceCount) {
  impl_->encoding_
    .push_back(make_unique<DrawCmd>(vertexStart, vertexCount,
                                    baseInstance, instanceCount));
}

void GrEncoder::drawIndexed(uint32_t indexStart,
                            uint32_t vertexCount,
                            int32_t vertexOffset,
                            uint32_t baseInstance,
                            uint32_t instanceCount) {
  impl_->encoding_
    .push_back(make_unique<DrawIxCmd>(indexStart, vertexCount, vertexOffset,
                                      baseInstance, instanceCount));
}

void GrEncoder::clearColor(Color value, uint32_t colorIndex) {
  impl_->encoding_
    .push_back(make_unique<ClearClCmd>(value, colorIndex));
}

void GrEncoder::clearDepth(float value) {
  impl_->encoding_
    .push_back(make_unique<ClearDpCmd>(value));
}

void GrEncoder::clearStencil(uint32_t value) {
  impl_->encoding_
    .push_back(make_unique<ClearScCmd>(value));
}

// -------------------------------------------------------------------------
// CpEncoder

CpEncoder::CpEncoder() : Encoder(Compute) { }

void CpEncoder::setState(CpState* state) {
  impl_->encoding_
    .push_back(make_unique<StateCpCmd>(state));
}

void CpEncoder::setDcTable(uint32_t tableIndex, uint32_t allocIndex) {
  impl_->encoding_
    .push_back(make_unique<DcTableCmd>(tableIndex, allocIndex));
}

void CpEncoder::dispatch(Size3 size) {
  impl_->encoding_
    .push_back(make_unique<DispatchCmd>(size));
}

// -------------------------------------------------------------------------
// TfEncoder

TfEncoder::TfEncoder() : Encoder(Transfer) { }
