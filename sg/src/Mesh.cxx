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

//
// Primitive
//

Primitive::Primitive() : impl_(make_unique<Impl>()) { }

Primitive::~Primitive() { }

// TODO: Consider allowing custom buffer size values
constexpr uint64_t Size = 1ULL << 21;

CG_NS::Buffer::Ptr Primitive::Impl::buffer_{CG_NS::device().buffer(Size)};
list<Primitive::Impl::Segment> Primitive::Impl::segments_{{0, Size}};

Primitive::Impl::~Impl() {
  for (const auto& de : attributes_)
    yieldEntry(de.second);

  yieldEntry(indices_);
}

void Primitive::Impl::setData(VxData semantic, uint32_t elementN,
                              uint32_t elementSize, const void* data) {

  DataEntry* entry;

  if (semantic != VxDataIndices) {
    auto it = find_if(attributes_.begin(), attributes_.end(),
                      [&](auto& att) { return att.first == semantic; });

    if (it != attributes_.end()) {
      yieldEntry(it->second);
      entry = &it->second;
    } else {
      attributes_.push_back({semantic, {}});
      entry = &attributes_.back().second;
    }

  } else {
    yieldEntry(indices_);
    entry = &indices_;
  }

  *entry = {UINT64_MAX, elementN, elementSize};
  const uint64_t size = elementN * elementSize;

  // Try to copy data to buffer
  auto copy = [&] {
    for (auto s = segments_.begin(); s != segments_.end(); s++) {
      if (s->size < size)
        continue;

      entry->offset = s->offset;
      buffer_->write(entry->offset, size, data);
      s->offset += size;
      s->size -= size;

      if (s->size == 0)
        segments_.erase(s);

      break;
    }
  };

  copy();

  if (entry->offset == UINT64_MAX) {
    // Copy failed, resize the buffer and try again
    const uint64_t bufSize = buffer_->size();
    if (!resizeBuffer(max(size, bufSize << 1)) &&
        (size >= bufSize || !resizeBuffer(size)))
      throw NoMemoryExcept("Failed to allocate space for Primitive");

    copy();

    assert(entry->offset != UINT64_MAX);
  }
}

void Primitive::Impl::yieldEntry(const DataEntry& dataEntry) {
  if (dataEntry.offset == UINT64_MAX)
    return;

  const uint64_t offset = dataEntry.offset;
  const uint64_t size = dataEntry.count * dataEntry.stride;
  const uint64_t end = offset + size;

  if (segments_.empty()) {
    segments_.push_back({offset, size});
    return;
  }

  auto next = segments_.begin();
  decltype(next) prev;
  for (; next != segments_.end() && next->offset < end; next++)
    prev = next;

  // Merge segments if they are contiguous, insert new segment otherwise
  if (next == segments_.begin()) {
    if (end == next->offset) {
      next->offset = offset;
      next->size += size;
    } else {
      segments_.push_front({offset, size});
    }

  } else if (next == segments_.end()) {
    const auto prevEnd = prev->offset + prev->size;
    if (prevEnd == offset)
      prev->size += size;
    else
      segments_.push_back({offset, size});

  } else {
    const auto prevEnd = prev->offset + prev->size;
    if (prevEnd == offset && end == next->offset) {
      prev->size += size + next->size;
      segments_.erase(next);
    } else if (prevEnd == offset) {
      prev->size += size;
    } else if (end == next->offset) {
      next->offset = offset;
      next->size += size;
    } else {
      segments_.insert(next, {offset, size});
    }
  }
}

bool Primitive::Impl::resizeBuffer(uint64_t newSize) {
  const uint64_t oldSize = buffer_->size();
  if (newSize == oldSize)
    return true;

  auto& dev = CG_NS::device();

  // Try to create a new buffer
  // XXX: This restricts the size to half the available memory
  CG_NS::Buffer::Ptr newBuf;
  try {
    buffer_ = dev.buffer(newSize);
  } catch (DeviceExcept&) {
    return false;
  }

  // Copy data to new buffer
  // TODO: Consider copying only used ranges
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

//
// Mesh
//

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

  // Copy primitives
  for (const auto& dp : data.primitives) {
    assert(dp.vxAccessors.size() > 0);

    primitives_.push_back({});
    auto& prim = primitives_.back();
    prim.topology = dp.topology;

    // Copy vertex attributes
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

    // Copy vertex indices
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

        prim.ixData.offset = copy(sz, dt);

        assert(prim.ixData.offset != UINT64_MAX);
      }

      prim.ixData.count = ia.elementN;
      prim.ixData.stride = ia.elementSize;
    }
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

  for (const auto& p : primitives_) {
    for (const auto& vd : p.vxData)
      yield(vd.second.offset, vd.second.count * vd.second.stride);

    if (p.ixData.offset != UINT64_MAX)
      yield(p.ixData.offset, p.ixData.count * p.ixData.stride);
  }

  // TODO: Consider shrinking the buffer, or provide a way to do so
}

uint32_t Mesh::Impl::primitiveCount() const {
  return primitives_.size();
}

CG_NS::Topology Mesh::Impl::topology(uint32_t primitive) const {
  if (primitive >= primitives_.size())
    throw invalid_argument("Mesh does not contain requested primitive");

  return primitives_[primitive].topology;
}

bool Mesh::Impl::canBind(VxType type, uint32_t primitive) const {
  if (primitive >= primitives_.size())
    throw invalid_argument("Mesh does not contain requested primitive");

  const auto& vxData = primitives_[primitive].vxData;
  return vxData.find(type) != vxData.end();
}

bool Mesh::Impl::canBind(VxType type) const {
  for (const auto& p : primitives_) {
    if (p.vxData.find(type) == p.vxData.end())
      return false;
  }
  return true;
}

bool Mesh::Impl::isIndexed(uint32_t primitive) const {
  if (primitive >= primitives_.size())
    throw invalid_argument("Mesh does not contain requested primitive");

  return primitives_[primitive].ixData.offset != UINT64_MAX;
}

bool Mesh::Impl::isIndexed() const {
  for (const auto& p : primitives_) {
    if (p.ixData.offset != UINT64_MAX)
      return true;
  }
  return false;
}

void Mesh::Impl::encodeVertexBuffer(CG_NS::GrEncoder& encoder,
                                    uint32_t inputIndex, VxType type,
                                    uint32_t primitive) {

  if (primitive >= primitives_.size())
    throw invalid_argument("Mesh does not contain requested primitive");

  const auto it = primitives_[primitive].vxData.find(type);
  if (it == primitives_[primitive].vxData.end())
    throw invalid_argument("Mesh does not contain requested vertex type");

  encoder.setVertexBuffer(buffer_.get(), it->second.offset, inputIndex);
}

void Mesh::Impl::encodeIndexBuffer(CG_NS::GrEncoder& encoder,
                                   uint32_t primitive) {

  if (primitive >= primitives_.size())
    throw invalid_argument("Mesh does not contain requested primitive");

  const auto& ixData = primitives_[primitive].ixData;
  if (ixData.offset == UINT64_MAX)
    return;

  CG_NS::IndexType type;
  switch (ixData.stride) {
  case 2:
    type = CG_NS::IndexTypeU16;
    break;
  case 4:
    type = CG_NS::IndexTypeU32;
    break;
  default:
    throw invalid_argument("Invalid stride for Mesh index buffer");
  }

  encoder.setIndexBuffer(buffer_.get(), ixData.offset, type);
}

void Mesh::Impl::encodeBindings(CG_NS::GrEncoder& encoder, uint32_t primitive) {
  if (primitive >= primitives_.size())
    throw invalid_argument("Mesh does not contain requested primitive");

  for (const auto& vd : primitives_[primitive].vxData)
    encoder.setVertexBuffer(buffer_.get(), vd.second.offset, vd.first);

  const auto& ixData = primitives_[primitive].ixData;
  if (ixData.offset == UINT64_MAX)
    return;

  CG_NS::IndexType type;
  switch (ixData.stride) {
  case 2:
    type = CG_NS::IndexTypeU16;
    break;
  case 4:
    type = CG_NS::IndexTypeU32;
    break;
  default:
    throw invalid_argument("Invalid stride for Mesh index buffer");
  }

  encoder.setIndexBuffer(buffer_.get(), ixData.offset, type);
}

void Mesh::Impl::encodeDraw(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                            uint32_t instanceCount, uint32_t primitive) {

  if (primitive >= primitives_.size())
    throw invalid_argument("Mesh does not contain requested primitive");

  const auto& vxData = primitives_[primitive].vxData.begin()->second;
  const auto& ixData = primitives_[primitive].ixData;

  if (ixData.offset != UINT64_MAX)
    encoder.drawIndexed(0, ixData.count, 0, baseInstance, instanceCount);
  else
    // XXX: This assumes that all vertex attributes have the same `count`
    encoder.draw(0, vxData.count, baseInstance, instanceCount);
}

void Mesh::Impl::encode(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                        uint32_t instanceCount, uint32_t primitive) {

  encodeBindings(encoder, primitive);
  encodeDraw(encoder, baseInstance, instanceCount, primitive);
}

void Mesh::Impl::encode(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                        uint32_t instanceCount) {

  uint32_t primitive = 0;
  do {
    encodeBindings(encoder, primitive);
    encodeDraw(encoder, baseInstance, instanceCount, primitive);
  } while (++primitive < primitives_.size());
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
