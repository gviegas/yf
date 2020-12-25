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
    data.vertex.data = new uint8_t[1000];
    data.vertex.size = 1000;
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
vector<Mesh::Impl::Segment> Mesh::Impl::segments_{{0, Len}};

Mesh::Impl::Impl(const Data& data) {
  // TODO: find a segment that can contain the data, copy data to buffer and
  // update segment list
}

Mesh::Impl::~Impl() {
  // TODO: update the segment list to reflect that the area previously
  // occupied by this mesh is again available for use
}

void Mesh::Impl::updateVertices(void* data, uint64_t offset, uint64_t size) {
  if (offset + size > vxSize_)
    throw invalid_argument("updateVertices() out of bounds");

  buffer_.write(vxOffset_ + offset, data, size);
}

void Mesh::Impl::updateIndices(void* data, uint32_t indexStart,
                               uint32_t indexCount) {

  if (indexStart + indexCount > ixCount_)
    throw invalid_argument("updateIndices() out of bounds");

  uint64_t offset = indexStart * ixStride_;
  uint64_t size = indexCount * ixStride_;
  buffer_.write(ixOffset_ + offset, data, size);
}

void Mesh::Impl::encodeBindings(CG_NS::GrEncoder& encoder,
                                uint32_t inputIndex) {
  // TODO
  throw runtime_error("Unimplemented");
}

void Mesh::Impl::encodeDraw(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                            uint32_t instanceCount) {
  // TODO
  throw runtime_error("Unimplemented");
}
