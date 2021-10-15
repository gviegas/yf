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

CG_NS::Topology Primitive::topology() const {
  return impl_->topology();
}

VxDataMask Primitive::dataMask() const {
  return impl_->dataMask();
}

const Material* Primitive::material() const {
  return impl_->material().get();
}

Primitive::Impl& Primitive::impl() {
  return *impl_;
}

// TODO: Consider allowing custom buffer size values
constexpr uint64_t Size = 1ULL << 21;

CG_NS::Buffer::Ptr Primitive::Impl::buffer_{CG_NS::device().buffer(Size)};
list<Primitive::Impl::Segment> Primitive::Impl::segments_{{0, Size}};

Primitive::Impl::~Impl() {
  for (const auto& de : attributes_)
    yieldEntry(de.second);

  yieldEntry(indices_);
}

CG_NS::Topology& Primitive::Impl::topology() {
  return topology_;
}

CG_NS::Topology Primitive::Impl::topology() const {
  return topology_;
}

VxDataMask Primitive::Impl::dataMask() const {
  return dataMask_;
}

Material::Ptr& Primitive::Impl::material() {
  return material_;
}

const Material* Primitive::Impl::material() const {
  return material_.get();
}

void Primitive::Impl::setData(VxData semantic, uint32_t elementN,
                              uint32_t elementSize, const void* data) {

  uint32_t vxCount;
  DataEntry* entry;

  if (semantic != VxDataIndices) {
    vxCount = elementN;
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
    vxCount = vxCount_;
  }

  *entry = {UINT64_MAX, elementN, elementSize};
  dataMask_ &= ~semantic;
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

  vxCount_ = vxCount;
  dataMask_ |= semantic;
}

void Primitive::Impl::encodeBindings(CG_NS::GrEncoder& encoder) {
  for (const auto& att : attributes_) {
    if (dataMask_ & att.first)
      encoder.setVertexBuffer(buffer_.get(), att.second.offset,
                              vxInputIndexFor(att.first));
  }

  if (dataMask_ & VxDataIndices) {
    switch (indices_.stride) {
    case 2:
      encoder.setIndexBuffer(buffer_.get(), indices_.offset,
                             CG_NS::IndexTypeU16);
      break;
    case 4:
      encoder.setIndexBuffer(buffer_.get(), indices_.offset,
                             CG_NS::IndexTypeU32);
      break;
    default:
      throw UnsupportedExcept("Unsupported type for index buffer");
    }
  }
}

void Primitive::Impl::encodeDraw(CG_NS::GrEncoder& encoder,
                                 uint32_t baseInstance,
                                 uint32_t instanceCount) {

  assert(dataMask_ & VxDataPosition);
  assert(none_of(attributes_.begin(), attributes_.end(), [&](auto& att) {
    return dataMask_ & att.first && vxCount_ != att.second.count;
  }));

  if (dataMask_ & VxDataIndices)
    encoder.drawIndexed(0, indices_.count, 0, baseInstance, instanceCount);
  else
    encoder.draw(0, vxCount_, baseInstance, instanceCount);
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

Primitive& Mesh::primitive(uint32_t index) {
  assert(index < impl_->primitives_.size());
  return *impl_->primitives_[index];
}

const Primitive& Mesh::primitive(uint32_t index) const {
  assert(index < impl_->primitives_.size());
  return *impl_->primitives_[index];
}

Primitive& Mesh::operator[](uint32_t index) {
  return primitive(index);
}

const Primitive& Mesh::operator[](uint32_t index) const {
  return primitive(index);
}

uint32_t Mesh::primitiveCount() const {
  return impl_->primitives_.size();
}

size_t Mesh::hash() const {
  return std::hash<decltype(impl_)>()(impl_);
}

Mesh::Impl& Mesh::impl() {
  return *impl_;
}

Mesh::Impl::Impl(const Data& data) {
  if (data.data.empty() || data.primitives.empty())
    throw invalid_argument("Invalid Mesh data");

  for (const auto& dp : data.primitives) {
    primitives_.push_back(make_unique<Primitive>());
    auto& prim = primitives_.back()->impl();

    prim.topology() = dp.topology;

    for (const auto& acc : dp.accessors)
      prim.setData(acc.semantic, acc.elementN, acc.elementSize,
                   &data.data[acc.dataIndex][acc.dataOffset]);

    if (dp.material)
      prim.material() = make_unique<Material>(*dp.material);
  }
}

Mesh::Impl::~Impl() { }
