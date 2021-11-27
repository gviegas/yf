//
// CG
// Encoder.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <utility>

#include "Encoder.h"
#include "Cmd.h"

using namespace CG_NS;
using namespace std;

class Encoder::Impl {
 public:
  Impl(Type type) : type_(type) { }

  void encode(unique_ptr<Cmd> cmd) {
    encoding_.push_back(move(cmd));
  }

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

//
// GrEncoder
//

GrEncoder::GrEncoder() : Encoder(Graphics) { }

void GrEncoder::setViewport(Viewport viewport, uint32_t viewportIndex) {
  impl_->encode(make_unique<ViewportCmd>(viewport, viewportIndex));
}

void GrEncoder::setScissor(Scissor scissor, uint32_t viewportIndex) {
  impl_->encode(make_unique<ScissorCmd>(scissor, viewportIndex));
}

void GrEncoder::setTarget(Target& target, const TargetOp& targetOp) {
  impl_->encode(make_unique<TargetCmd>(target, targetOp));
}

void GrEncoder::setState(GrState& state) {
  impl_->encode(make_unique<StateGrCmd>(state));
}

void GrEncoder::setDcTable(uint32_t tableIndex, uint32_t allocIndex) {
  impl_->encode(make_unique<DcTableCmd>(tableIndex, allocIndex));
}

void GrEncoder::setVertexBuffer(Buffer& buffer, uint64_t offset,
                                uint32_t inputIndex) {

  impl_->encode(make_unique<VxBufferCmd>(buffer, offset, inputIndex));
}

void GrEncoder::setIndexBuffer(Buffer& buffer, uint64_t offset,
                               IndexType type) {

  impl_->encode(make_unique<IxBufferCmd>(buffer, offset, type));
}

void GrEncoder::draw(uint32_t vertexStart, uint32_t vertexCount,
                     uint32_t baseInstance, uint32_t instanceCount) {

  impl_->encode(make_unique<DrawCmd>(vertexStart, vertexCount,
                                     baseInstance, instanceCount));
}

void GrEncoder::drawIndexed(uint32_t indexStart, uint32_t vertexCount,
                            int32_t vertexOffset, uint32_t baseInstance,
                            uint32_t instanceCount) {

  impl_->encode(make_unique<DrawIxCmd>(indexStart, vertexCount, vertexOffset,
                                       baseInstance, instanceCount));
}

void GrEncoder::synchronize() {
  impl_->encode(make_unique<SyncCmd>());
}

//
// CpEncoder
//

CpEncoder::CpEncoder() : Encoder(Compute) { }

void CpEncoder::setState(CpState& state) {
  impl_->encode(make_unique<StateCpCmd>(state));
}

void CpEncoder::setDcTable(uint32_t tableIndex, uint32_t allocIndex) {
  impl_->encode(make_unique<DcTableCmd>(tableIndex, allocIndex));
}

void CpEncoder::dispatch(Size3 size) {
  impl_->encode(make_unique<DispatchCmd>(size));
}

void CpEncoder::synchronize() {
  impl_->encode(make_unique<SyncCmd>());
}

//
// TfEncoder
//

TfEncoder::TfEncoder() : Encoder(Transfer) { }

void TfEncoder::copy(Buffer& dst, uint64_t dstOffset,
                     Buffer& src, uint64_t srcOffset,
                     uint64_t size) {

  impl_->encode(make_unique<CopyBBCmd>(dst, dstOffset, src, srcOffset, size));
}

void TfEncoder::copy(Image* dst, Offset2 dstOffset,
                     uint32_t dstLayer, uint32_t dstLevel,
                     Image* src, Offset2 srcOffset,
                     uint32_t srcLayer, uint32_t srcLevel,
                     Size2 size, uint32_t layerCount) {

  impl_->encode(make_unique<CopyIICmd>(dst, dstOffset, dstLayer, dstLevel,
                                       src, srcOffset, srcLayer, srcLevel,
                                       size, layerCount));
}
