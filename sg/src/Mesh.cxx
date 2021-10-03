//
// SG
// Mesh.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cassert>

#include "yf/cg/Device.h"

#include "MeshImpl.h"
#include "DataGLTF.h"
#include "yf/Except.h"

using namespace SG_NS;
using namespace std;

Mesh::Mesh(const string& pathname, size_t index) {
  // TODO: Consider checking the type of the file.
  Data data;
  loadGLTF(data, pathname, index);
  impl_ = make_unique<Impl>(data);
}

Mesh::Mesh(ifstream& stream, size_t index) {
  // TODO: Consider checking the type of the file.
  Data data;
  loadGLTF(data, stream, index);
  impl_ = make_unique<Impl>(data);
}

Mesh::Mesh(const Data& data) : impl_(make_unique<Impl>(data)) { }

Mesh::~Mesh() { }

size_t Mesh::hash() const {
  return std::hash<decltype(impl_)>()(impl_);
}

Mesh::Impl& Mesh::impl() {
  return *impl_;
}

// TODO: Consider allowing custom length values
constexpr uint64_t Length = 1ULL << 21;

CG_NS::Buffer::Ptr Mesh::Impl::buffer_{CG_NS::device().buffer(Length)};
list<Mesh::Impl::Segment> Mesh::Impl::segments_{{0, Length}};

Mesh::Impl::Impl(const Data& data) {
  if (data.data.empty() || data.primitives.empty())
    throw invalid_argument("Invalid mesh data");

  // Find a segment that can contain data of a given size, copy data to
  // buffer and update segment list
  auto copy = [&](uint64_t size, const void* data) -> uint64_t {
    for (auto s = segments_.begin(); s != segments_.end(); s++) {
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

  // Copy data of each primitive
  for (const auto& dp : data.primitives) {
    assert(dp.vxAccessors.size() > 0);

    primitives_.push_back({});
    auto& prim = primitives_.back();
    prim.topology = dp.topology;

    // Copy vertex attribute data
    for (const auto& va : dp.vxAccessors) {
      assert(va.second.dataIndex < data.data.size());
      assert(va.second.elementN > 0);
      assert(va.second.elementSize > 0);

      const uint64_t sz = va.second.elementN * va.second.elementSize;
      const void* dt = &data.data[va.second.dataIndex][va.second.dataOffset];
      uint64_t off = copy(sz, dt);

      if (off == UINT64_MAX) {
        if (!resizeBuffer(max(sz, buffer_->size() << 1)) &&
            (sz >= buffer_->size() || !resizeBuffer(sz)))
          throw NoMemoryExcept("Failed to allocate space for Mesh");

        off = copy(sz, dt);

        assert(off != UINT64_MAX);
      }

      prim.vxData.emplace(va.first, DataEntry{off, va.second.elementN,
                                              va.second.elementSize});
    }

    // Copy index data
    const auto& ia = dp.ixAccessor;
    if (ia.dataIndex != UINT32_MAX) {
      assert(ia.dataIndex < data.data.size());
      assert(ia.elementN > 0);
      assert(ia.elementSize > 0);

      const uint64_t sz = ia.elementN * ia.elementSize;
      const void* dt = &data.data[ia.dataIndex][ia.dataOffset];
      prim.ixData.offset = copy(sz, dt);

      if (prim.ixData.offset == UINT64_MAX) {
        if (!resizeBuffer(max(sz, buffer_->size() << 1)) &&
            (sz >= buffer_->size() || !resizeBuffer(sz)))
          throw NoMemoryExcept("Failed to allocate space for Mesh");
      }

      prim.ixData.offset = copy(sz, dt);

      assert(prim.ixData.offset != UINT64_MAX);
    }

    prim.ixData.count = ia.elementN;
    prim.ixData.stride = ia.elementSize;
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
    for (; next != segments_.end() && next->offset < offset+size; next++)
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

  for (const auto& vd : vxData_)
    yield(vd.second.offset, vd.second.count * vd.second.stride);

  if (ixData_.offset != UINT64_MAX)
    yield(ixData_.offset, ixData_.count * ixData_.stride);

  // TODO: Consider shrinking the buffer, or provide a way to do so
}

bool Mesh::Impl::canBind(VxType type) const {
  return vxData_.find(type) != vxData_.end();
}

bool Mesh::Impl::isIndexed() const {
  return ixData_.offset != UINT64_MAX;
}

void Mesh::Impl::encodeVertexBuffer(CG_NS::GrEncoder& encoder, VxType type,
                                    uint32_t inputIndex) {

  const auto it = vxData_.find(type);
  if (it == vxData_.end())
    throw invalid_argument("Mesh does not contain requested vertex type");

  encoder.setVertexBuffer(buffer_.get(), it->second.offset, inputIndex);
}

void Mesh::Impl::encodeIndexBuffer(CG_NS::GrEncoder& encoder) {
  if (ixData_.offset == UINT64_MAX)
    return;

  CG_NS::IndexType type;
  switch (ixData_.stride) {
  case 2:
    type = CG_NS::IndexTypeU16;
    break;
  case 4:
    type = CG_NS::IndexTypeU32;
    break;
  default:
    throw invalid_argument("Invalid stride for Mesh index buffer");
  }

  encoder.setIndexBuffer(buffer_.get(), ixData_.offset, type);
}

void Mesh::Impl::encodeBindings(CG_NS::GrEncoder& encoder) {
  for (const auto& vd : vxData_)
    encoder.setVertexBuffer(buffer_.get(), vd.second.offset, vd.first);

  encodeIndexBuffer(encoder);
}

void Mesh::Impl::encodeDraw(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                            uint32_t instanceCount) {

  if (ixData_.offset != UINT64_MAX)
    encoder.drawIndexed(0, ixData_.count, 0, baseInstance, instanceCount);
  else
    // XXX: Assuming all vertex attributes have the same count
    encoder.draw(0, vxData_.begin()->second.count, baseInstance, instanceCount);
}

void Mesh::Impl::encode(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                        uint32_t instanceCount) {

  encodeBindings(encoder);
  encodeDraw(encoder, baseInstance, instanceCount);
}

bool Mesh::Impl::resizeBuffer(uint64_t newSize) {
  const auto oldSize = buffer_->size();
  if (newSize == oldSize)
    return true;

  auto& dev = CG_NS::device();

  // Try to create a new buffer
  // XXX: This restricts the buffer size to half the available memory
  CG_NS::Buffer::Ptr newBuf;
  try {
    newBuf = dev.buffer(newSize);
  } catch (DeviceExcept&) {
    return false;
  }

  // Copy data to new buffer
  CG_NS::TfEncoder enc;
  enc.copy(newBuf.get(), 0, buffer_.get(), 0, buffer_->size());
  auto& que = dev.queue(CG_NS::Queue::Transfer);
  auto cb = que.cmdBuffer();
  cb->encode(enc);
  cb->enqueue();
  que.submit();

  buffer_.reset(newBuf.release());

  // Update segment list
  if (newSize > oldSize) {
    if (segments_.empty()) {
      segments_.push_front({oldSize, newSize - oldSize});
    } else {
      auto& back = segments_.back();
      if (back.offset + back.size == oldSize)
        back.size = newSize - back.offset;
      else
        segments_.push_back({oldSize, newSize - oldSize});
    }
  } else {
    if (segments_.empty())
      throw runtime_error("Bad buffer resize");

    auto& back = segments_.back();
    if (back.offset + back.size != oldSize || back.offset > newSize)
      throw runtime_error("Bad buffer resize");

    if (back.offset == newSize)
      segments_.pop_back();
    else
      back.size = newSize - back.offset;
  }

  return true;
}
