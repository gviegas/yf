//
// SG
// Mesh.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/Except.h"

#include "yf/cg/Device.h"

#include "MeshImpl.h"
#include "DataOBJ.h"

using namespace SG_NS;
using namespace std;

Mesh::Mesh(FileType fileType, const wstring& meshFile) {
  Data data;

  switch (fileType) {
  case Internal:
    // TODO
    throw runtime_error("Mesh::Internal unimplemented");
  case Collada:
    // TODO
    throw runtime_error("Mesh::Collada unimplemented");
  case Obj:
    loadOBJ(data, meshFile);
    break;
  default:
    throw invalid_argument("Invalid Mesh file type");
  }

  impl_ = make_unique<Impl>(data);
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
  : vxOffset_(UINT64_MAX), vxCount_(data.vxCount), vxStride_(data.vxStride),
    ixOffset_(UINT64_MAX), ixCount_(data.ixCount), ixStride_(data.ixStride) {

  // Find a segment that can contain data of a given size, copy data to
  // buffer and update segment list
  auto copy = [&](uint64_t size, const void* data) -> uint64_t {
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
  vxOffset_ = copy(vxSize, data.vxData.get());
  if (vxOffset_ == UINT64_MAX)
    // TODO: create a larger buffer and transfer data
    throw runtime_error("Mesh buffer resize unimplemented");

  if (ixCount_ > 0) {
    const uint64_t ixSize = ixCount_ * ixStride_;
    ixOffset_ = copy(ixSize, data.ixData.get());
    if (ixOffset_ == UINT64_MAX)
      // TODO: create a larger buffer and transfer data
      throw runtime_error("Mesh buffer resize unimplemented");
  }
}

Mesh::Impl::~Impl() {
  // Update the segment list to reflect that a given buffer area is once
  // again available for use
  auto yield = [&](uint64_t offset, uint64_t size) {
    if (segments_.empty()) {
      segments_.push_back({offset, size});
      return;
    }

    auto next = segments_.begin();
    decltype(next) prev;
    for (; next != segments_.end() && next->offset < offset+size; ++next)
      prev = next;

    // Merge segments if they are contiguous, insert new segment otherwise
    if (next == segments_.begin()) {
      if (offset+size == next->offset) {
        next->offset = offset;
        next->size += size;
      } else {
        segments_.push_front({offset, size});
      }
    } else if (next == segments_.end()) {
      if (prev->offset+prev->size == offset)
        prev->size += size;
      else
        segments_.push_back({offset, size});
    } else {
      if (prev->offset+prev->size == offset && offset+size == next->offset) {
        prev->size += size + next->size;
        segments_.erase(next);
      } else if (prev->offset+prev->size == offset) {
        prev->size += size;
      } else if (offset+size == next->offset) {
        next->offset = offset;
        next->size += size;
      } else {
        segments_.insert(next, {offset, size});
      }
    }
  };

  const uint64_t vxSize = vxCount_ * vxStride_;
  yield(vxOffset_, vxSize);

  if (ixCount_ > 0) {
    const uint64_t ixSize = ixCount_ * ixStride_;
    yield(ixOffset_, ixSize);
  }
}

void Mesh::Impl::updateVertices(uint32_t vertexStart, uint32_t vertexCount,
                                const void* data) {

  if (vertexStart + vertexCount > vxCount_)
    throw invalid_argument("updateVertices() out of bounds");

  uint64_t offset = vertexStart * vxStride_;
  uint64_t size = vertexCount * vxStride_;
  buffer_->write(vxOffset_ + offset, size, data);
}

void Mesh::Impl::updateIndices(uint32_t indexStart, uint32_t indexCount,
                               const void* data) {

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
