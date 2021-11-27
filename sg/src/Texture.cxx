//
// SG
// Texture.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cassert>

#include "yf/cg/Device.h"
#include "yf/cg/Encoder.h"

#include "TextureImpl.h"
#include "DataPNG.h"
#include "yf/Except.h"

using namespace SG_NS;
using namespace std;

Texture::Texture(const string& pathname) {
  // TODO: Consider checking the type of the file.
  Data data;
  loadPNG(data, pathname);
  impl_ = make_unique<Impl>(data);
}

Texture::Texture(ifstream& stream) {
  // TODO: Consider checking the type of the file.
  Data data;
  loadPNG(data, stream);
  impl_ = make_unique<Impl>(data);
}

Texture::Texture(const Data& data) : impl_(make_unique<Impl>(data)) { }

Texture::Texture(const Texture& other, const CG_NS::Sampler& sampler,
                 TexCoordSet coordSet)
  : impl_(make_unique<Impl>(*other.impl_, sampler, coordSet)) { }

Texture::Texture(const Texture& other)
  : impl_(make_unique<Impl>(*other.impl_)) { }

Texture& Texture::operator=(const Texture& other) {
  *impl_ = *other.impl_;
  return *this;
}

Texture::~Texture() { }

CG_NS::Sampler& Texture::sampler() {
  return impl_->sampler();
}

const CG_NS::Sampler& Texture::sampler() const {
  return impl_->sampler();
}

TexCoordSet& Texture::coordSet() {
  return impl_->coordSet();
}

TexCoordSet Texture::coordSet() const {
  return impl_->coordSet();
}

size_t Texture::hash() const {
  return std::hash<decltype(impl_)>()(impl_);
}

Texture::Impl& Texture::impl() {
  return *impl_;
}

// TODO: Consider allowing custom layers value
constexpr uint32_t Layers = 16;

Texture::Impl::Resources Texture::Impl::resources_{};

Texture::Impl::Impl(const Data& data)
  : key_{data.format, data.size, data.levels, data.samples},
    layer_(UINT32_MAX), sampler_(data.sampler), coordSet_(data.coordSet) {

  auto it = resources_.find(key_);

  // Create a new image if none matches the data parameters or if more
  // layers are needed
  if (it == resources_.end()) {
    auto& dev = CG_NS::device();

    auto res = resources_.emplace(key_, Resource{
      dev.image(data.format, data.size, Layers, data.levels, data.samples),
      {vector<uint32_t>(Layers, 0), Layers, 0}});

    it = res.first;

  } else if (it->second.layers.remaining == 0) {
    if (!setLayerCount(it->second, it->second.image->layers() << 1) &&
        !setLayerCount(it->second, it->second.image->layers() + 1))
      throw NoMemoryExcept("Failed to allocate space for texture object");
  }

  auto& resource = it->second;

  // Find an unused layer to copy this data to
  vector<uint32_t>& refCounts = resource.layers.refCounts;
  uint32_t& current = resource.layers.current;
  do {
    if (refCounts[current] == 0) {
      layer_ = current;
      refCounts[current]++;
    }
    current = (current + 1) % refCounts.size();
  } while (layer_ == UINT32_MAX);
  resource.layers.remaining--;

  // Copy the data
  CG_NS::Image& image = *resource.image;
  CG_NS::Size2 size = data.size;
  const uint32_t txSz = image.texelSize();
  const char* bytes = data.data.get();

  // TODO: Check if this works as expected
  for (uint32_t i = 0; i < data.levels; i++) {
    image.write({0}, size, layer_, i, bytes);
    bytes += size.width * size.height * txSz;
    size.width = max(1U, size.width >> 1);
    size.height = max(1U, size.height >> 1);
  }
}

Texture::Impl::Impl(const Impl& other, const CG_NS::Sampler& sampler,
                    TexCoordSet coordSet)
  : key_(other.key_), layer_(other.layer_), sampler_(sampler),
    coordSet_(coordSet) {

  // Shared
  auto& resource = resources_.find(key_)->second;
  resource.layers.refCounts[layer_]++;
}

Texture::Impl::Impl(const Impl& other)
  : key_(other.key_), layer_(other.layer_), sampler_(other.sampler_),
    coordSet_(other.coordSet_) {

  // Shared
  auto& resource = resources_.find(key_)->second;
  resource.layers.refCounts[layer_]++;
}

Texture::Impl& Texture::Impl::operator=(const Impl& other) {
  auto& otherRes = resources_.find(other.key_)->second;
  otherRes.layers.refCounts[other.layer_]++;

  if (key_ == other.key_) {
    otherRes.layers.refCounts[layer_]--;
  } else {
    auto& thisRes = resources_.find(key_)->second;
    thisRes.layers.refCounts[layer_]--;
    key_ = other.key_;
  }

  layer_ = other.layer_;
  sampler_ = other.sampler_;
  coordSet_ = other.coordSet_;
  return *this;
}

Texture::Impl::~Impl() {
  auto& resource = resources_.find(key_)->second;

  if (--resource.layers.refCounts[layer_] == 0) {
    // Yield the layer used by the texture, destroying the resource if all of
    // its layers become unused as a result
    if (++resource.layers.remaining == resource.layers.refCounts.size())
      resources_.erase(key_);
    else
      resource.layers.current = layer_;
  }
}

CG_NS::Sampler& Texture::Impl::sampler() {
  return sampler_;
}

const CG_NS::Sampler& Texture::Impl::sampler() const {
  return sampler_;
}

TexCoordSet& Texture::Impl::coordSet() {
  return coordSet_;
}

TexCoordSet Texture::Impl::coordSet() const {
  return coordSet_;
}

void Texture::Impl::updateImage(CG_NS::Offset2 offset, CG_NS::Size2 size,
                                uint32_t level, const void* data) {

  auto& image = *resources_.find(key_)->second.image;
  image.write(offset, size, layer_, level, data);
}

void Texture::Impl::copy(CG_NS::DcTable& dcTable, uint32_t allocation,
                         CG_NS::DcId id, uint32_t element, uint32_t level) {

  auto& image = *resources_.find(key_)->second.image;
  dcTable.write(allocation, id, element, image, layer_, level, sampler_);
}

bool Texture::Impl::setLayerCount(Resource& resource, uint32_t newCount) {
  const auto oldCount = resource.image->layers();
  if (newCount == oldCount)
    return true;

  auto& dev = CG_NS::device();

  // Try to create a new image
  CG_NS::Image::Ptr newImg;
  try {
    newImg = dev.image(resource.image->format(), resource.image->size(),
                       newCount, resource.image->levels(),
                       resource.image->samples());
  } catch (DeviceExcept&) {
    return false;
  }

  // Copy data to new image
  CG_NS::TfEncoder enc;
  const auto cpyCount = min(newCount, resource.image->layers());
  const auto cpySize = resource.image->size();
  for (uint32_t i = 0; i < resource.image->levels(); i++)
    enc.copy(*newImg, {0}, 0, i, *resource.image, {0}, 0, i,
             {cpySize.width >> i, cpySize.height >> i}, cpyCount);

  auto& que = dev.queue(CG_NS::Queue::Transfer);
  auto cb = que.cmdBuffer();
  cb->encode(enc);
  cb->enqueue();
  que.submit();

  resource.image.reset(newImg.release());

  // Update resource
  if (newCount > oldCount) {
    resource.layers.refCounts.resize(newCount, 0);
    resource.layers.remaining += newCount - oldCount;
  } else {
    for (auto i = oldCount-1; i >= newCount; i--) {
      if (resource.layers.refCounts[i] != 0)
        throw runtime_error("Bad texture layer count");
    }
    resource.layers.refCounts.resize(newCount);
    resource.layers.remaining -= oldCount - newCount;
    resource.layers.current %= newCount;
  }

  return true;
}
