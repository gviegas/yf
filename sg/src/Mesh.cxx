//
// SG
// Mesh.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cassert>

#include "yf/Except.h"

#include "yf/cg/Device.h"

#include "MeshImpl.h"
#include "DataGLTF.h"
#include "DataOBJ.h"

using namespace SG_NS;
using namespace std;

Mesh::Mesh(FileType fileType, const wstring& meshFile, size_t index) {
  Data data;

  switch (fileType) {
  case Internal:
    // TODO
    throw runtime_error("Mesh::Internal unimplemented");
  case Gltf:
    loadGLTF(data, meshFile, index);
    break;
  case Obj:
    loadOBJ(data, meshFile);
    break;
  default:
    throw invalid_argument("Invalid Mesh file type");
  }

  impl_ = make_unique<Impl>(data);
}

Mesh::~Mesh() { }

Mesh::Mesh(const Data& data) : impl_(make_shared<Impl>(data)) { }

Mesh::Mesh(const Mesh& other) : impl_(other.impl_) { }

Mesh& Mesh::operator=(const Mesh& other) {
  impl_ = other.impl_;
  return *this;
}

Mesh::Impl& Mesh::impl() {
  return *impl_;
}

// TODO: consider allowing custom length values
constexpr uint64_t Length = 1ULL << 21;

CG_NS::Buffer::Ptr Mesh::Impl::buffer_{CG_NS::device().buffer(Length)};
list<Mesh::Impl::Segment> Mesh::Impl::segments_{{0, Length}};

Mesh::Impl::Impl(const Data& data) {
  if (data.data.empty() || data.vxAccessors.empty())
    throw invalid_argument("Invalid mesh data");

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

  // Copy vertex data
  for (const auto& va : data.vxAccessors) {
    assert(va.second.dataIndex < data.data.size());
    assert(va.second.elementN > 0);
    assert(va.second.elementSize > 0);

    const uint64_t sz = va.second.elementN * va.second.elementSize;
    const void* dt = data.data[va.second.dataIndex].get()+va.second.dataOffset;
    uint64_t off = copy(sz, dt);

    if (off == UINT64_MAX) {
      if (!resizeBuffer(max(sz, buffer_->size_ << 1)) &&
          (sz >= buffer_->size_ || !resizeBuffer(sz)))
        throw NoMemoryExcept("Failed to allocate space for mesh object");

      off = copy(sz, dt);

      assert(off != UINT64_MAX);
    }

    vxData_.emplace(va.first, DataEntry{off, va.second.elementN,
                                        va.second.elementSize});
  }

  // Copy index data
  if (data.ixAccessor.dataIndex != UINT32_MAX) {
    assert(data.ixAccessor.dataIndex < data.data.size());
    assert(data.ixAccessor.elementN > 0);
    assert(data.ixAccessor.elementSize > 0);

    const uint64_t sz = data.ixAccessor.elementN * data.ixAccessor.elementSize;
    const void* dt = data.data[data.ixAccessor.dataIndex].get()+
                     data.ixAccessor.dataOffset;
    ixData_.offset = copy(sz, dt);

    if (ixData_.offset == UINT64_MAX) {
      if (!resizeBuffer(max(sz, buffer_->size_ << 1)) &&
          (sz >= buffer_->size_ || !resizeBuffer(sz)))
        throw NoMemoryExcept("Failed to allocate space for mesh object");

      ixData_.offset = copy(sz, dt);

      assert(ixData_.offset != UINT64_MAX);
    }

    ixData_.count = data.ixAccessor.elementN;
    ixData_.stride = data.ixAccessor.elementSize;
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

  for (const auto& vd : vxData_)
    yield(vd.second.offset, vd.second.count * vd.second.stride);

  if (ixData_.offset != UINT64_MAX)
    yield(ixData_.offset, ixData_.count * ixData_.stride);

  // TODO: consider shrinking the buffer, or provide a way to do so
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
    // XXX: assuming all vertex attributes have the same count
    encoder.draw(0, vxData_.begin()->second.count, baseInstance, instanceCount);
}

void Mesh::Impl::encode(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                        uint32_t instanceCount) {

  encodeBindings(encoder);
  encodeDraw(encoder, baseInstance, instanceCount);
}

bool Mesh::Impl::resizeBuffer(uint64_t newSize) {
  const auto oldSize = buffer_->size_;
  if (newSize == oldSize)
    return true;

  auto& dev = CG_NS::device();

  // Try to create a new buffer
  // XXX: this restricts the buffer size to half the available memory
  CG_NS::Buffer::Ptr newBuf;
  try {
    newBuf = dev.buffer(newSize);
  } catch (DeviceExcept&) {
    return false;
  }

  // Copy data to new buffer
  CG_NS::TfEncoder enc;
  enc.copy(newBuf.get(), 0, buffer_.get(), 0, buffer_->size_);
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
