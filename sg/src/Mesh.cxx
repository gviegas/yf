//
// SG
// Mesh.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/Except.h"

#include "yf/cg/Device.h"

#include "MeshImpl.h"

using namespace SG_NS;
using namespace std;

Mesh::Mesh(FileType fileType, const wstring& meshFile) {
  Data data;

  // XXX: for testing
  auto test = [&] {
    data.vertex.data = new float[8*20];
    data.vertex.count = 20;
    data.vertex.stride = 4*8;
    data.index.data = new uint16_t[300];
    data.index.count = 300;
    data.index.stride = 2;
  };
  test();
  goto testing;

  switch (fileType) {
  case Internal:
    // TODO
    throw runtime_error("Mesh::Internal unimplemented");
  case Collada:
    // TODO
    throw runtime_error("Mesh::Collada unimplemented");
  case Obj:
    // TODO
    throw runtime_error("Mesh::Obj unimplemented");
  default:
    throw invalid_argument("Invalid Mesh file type");
  }

  // XXX
  testing:

  impl_ = make_unique<Impl>(data);

  // XXX
  delete[] reinterpret_cast<uint8_t*>(data.vertex.data);
  delete[] reinterpret_cast<uint16_t*>(data.index.data);
}

Mesh::~Mesh() { }

Mesh::Mesh(const Data& data) : impl_(make_unique<Impl>(data)) { }

Mesh::Impl& Mesh::impl() {
  return *impl_;
}

constexpr const uint64_t Len = 1<<24;

CG_NS::Buffer::Ptr Mesh::Impl::buffer_{CG_NS::Device::get().makeBuffer(Len)};
list<Mesh::Impl::Segment> Mesh::Impl::segments_{{0, Len}};

Mesh::Impl::Impl(const Data& data)
  : vxOffset_(UINT64_MAX), vxCount_(data.vertex.count),
    vxStride_(data.vertex.stride), ixOffset_(UINT64_MAX),
    ixCount_(data.index.count), ixStride_(data.index.stride) {

  // Find a segment that can contain the data, copy data to buffer and
  // update segment list
  auto copy = [&](uint64_t size, void* data) -> uint64_t {
    for (auto s = segments_.begin(); s != segments_.end(); ++s) {
      if (s->size < size)
        continue;

      const uint64_t offset = s->offset;
      buffer_->write(offset, size, data);
      s->offset += size;
      s->size -= size;

      if (s->size == 0)
        segments_.erase(s);

      return offset;
    }
    return UINT64_MAX;
  };

  const uint64_t vxSize = vxCount_ * vxStride_;
  vxOffset_ = copy(vxSize, data.vertex.data);
  if (vxOffset_ == UINT64_MAX)
    // TODO: create a larger buffer and transfer data
    throw runtime_error("Mesh buffer resize unimplemented");

  if (ixCount_ > 0) {
    const uint64_t ixSize = ixCount_ * ixStride_;
    ixOffset_ = copy(ixSize, data.index.data);
    if (ixOffset_ == UINT64_MAX)
      // TODO: create a larger buffer and transfer data
      throw runtime_error("Mesh buffer resize unimplemented");
  }
}

Mesh::Impl::~Impl() {
  // TODO: update the segment list to reflect that the area previously
  // occupied by this mesh is again available for use
}

void Mesh::Impl::updateVertices(void* data, uint32_t vertexStart,
                                uint32_t vertexCount) {

  if (vertexStart + vertexCount > vxCount_)
    throw invalid_argument("updateVertices() out of bounds");

  uint64_t offset = vertexStart * vxStride_;
  uint64_t size = vertexCount * vxStride_;
  buffer_->write(vxOffset_ + offset, size, data);
}

void Mesh::Impl::updateIndices(void* data, uint32_t indexStart,
                               uint32_t indexCount) {

  if (indexStart + indexCount > ixCount_)
    throw invalid_argument("updateIndices() out of bounds");

  uint64_t offset = indexStart * ixStride_;
  uint64_t size = indexCount * ixStride_;
  buffer_->write(ixOffset_ + offset, size, data);
}

void Mesh::Impl::encodeBindings(CG_NS::GrEncoder& encoder,
                                uint32_t inputIndex) {

  encoder.setVertexBuffer(buffer_.get(), vxOffset_, inputIndex);

  if (ixCount_ > 0) {
    // XXX: the code that provides the `Data` for the mesh must ensure that
    // stride is valid
    auto type = ixStride_ == 2 ? CG_NS::IndexTypeU16 : CG_NS::IndexTypeU32;
    encoder.setIndexBuffer(buffer_.get(), ixOffset_, type);
  }
}

void Mesh::Impl::encodeDraw(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                            uint32_t instanceCount) {
  if (ixCount_ > 0)
    encoder.drawIndexed(0, ixCount_, 0, baseInstance, instanceCount);
  else
    encoder.draw(0, vxCount_, baseInstance, instanceCount);
}
