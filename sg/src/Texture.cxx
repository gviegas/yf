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

Texture::Texture(const wstring& pathname) {
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

Texture::~Texture() { }

size_t Texture::hash() const {
  return std::hash<decltype(impl_)>()(impl_);
}

Texture::Impl& Texture::impl() {
  return *impl_;
}

// TODO: consider allowing custom layers value
constexpr uint32_t Layers = 16;

Texture::Impl::Resources Texture::Impl::resources_{};

Texture::Impl::Impl(const Data& data)
  : key_(data.format, data.size, data.levels, data.samples),
    layer_(UINT32_MAX) {

  auto it = resources_.find(key_);

  // Create a new image if none matches the data parameters or if more
  // layers are needed
  if (it == resources_.end()) {
    auto& dev = CG_NS::device();

    auto res = resources_.emplace(key_, Resource{
      dev.image(data.format, data.size, Layers, data.levels, data.samples),
      {vector<bool>(Layers, true), Layers, 0}});

    it = res.first;

  } else if (it->second.layers.remaining == 0) {
    if (!setLayerCount(it->second, it->second.image->layers_ << 1) &&
        !setLayerCount(it->second, it->second.image->layers_ + 1))
      throw NoMemoryExcept("Failed to allocate space for texture object");
  }

  auto& resource = it->second;

  // Find an unused layer to copy this data to
  vector<bool>& unused = resource.layers.unused;
  uint32_t& current = resource.layers.current;
  do {
    if (unused[current]) {
      layer_ = current;
      unused[current] = false;
    }
    current = (current + 1) % unused.size();
  } while (layer_ == UINT32_MAX);
  resource.layers.remaining--;

  // Copy the data
  CG_NS::Image& image = *resource.image;
  CG_NS::Size2 size = data.size;
  uint8_t* bytes = data.data.get();
  // TODO: check if this works as expected
  for (uint32_t i = 0; i < data.levels; i++) {
    image.write({0}, size, layer_, i, bytes);
    bytes += (image.bitsPerTexel_ >> 3) * size.width * size.height;
    size.width = max(1U, size.width >> 1);
    size.height = max(1U, size.height >> 1);
  }
}

Texture::Impl::~Impl() {
  auto& resource = resources_.find(key_)->second;

  // Yield the layer used by this texture, destroying the resource if all of
  // its layers become unused as a result
  resource.layers.unused[layer_] = true;
  if (++resource.layers.remaining == resource.layers.unused.size())
    resources_.erase(key_);
  else
    resource.layers.current = layer_;
}

void Texture::Impl::updateImage(CG_NS::Offset2 offset, CG_NS::Size2 size,
                                uint32_t level, const void* data) {

  auto& image = *resources_.find(key_)->second.image;
  image.write(offset, size, layer_, level, data);
}

void Texture::Impl::copy(CG_NS::DcTable& dcTable, uint32_t allocation,
                         CG_NS::DcId id, uint32_t element, uint32_t level,
                         CG_NS::Sampler* sampler) {

  auto& image = *resources_.find(key_)->second.image;
  if (sampler)
    dcTable.write(allocation, id, element, image, layer_, level, *sampler);
  else
    dcTable.write(allocation, id, element, image, layer_, level);
}

bool Texture::Impl::setLayerCount(Resource& resource, uint32_t newCount) {
  const auto oldCount = resource.image->layers_;
  if (newCount == oldCount)
    return true;

  auto& dev = CG_NS::device();

  // Try to create a new image
  CG_NS::Image::Ptr newImg;
  try {
    newImg = dev.image(resource.image->format_, resource.image->size_,
                       newCount, resource.image->levels_,
                       resource.image->samples_);
  } catch (DeviceExcept&) {
    return false;
  }

  // Copy data to new image
  CG_NS::TfEncoder enc;
  const auto cpyCount = min(newCount, resource.image->layers_);
  const auto cpySize = resource.image->size_;
  for (uint32_t i = 0; i < resource.image->levels_; i++)
    enc.copy(newImg.get(), {0}, 0, i, resource.image.get(), {0}, 0, i,
             {cpySize.width >> i, cpySize.height >> i}, cpyCount);

  auto& que = dev.queue(CG_NS::Queue::Transfer);
  auto cb = que.cmdBuffer();
  cb->encode(enc);
  cb->enqueue();
  que.submit();

  resource.image.reset(newImg.release());

  // Update resource
  if (newCount > oldCount) {
    resource.layers.unused.resize(newCount, true);
    resource.layers.remaining += newCount - oldCount;
  } else {
    for (auto i = oldCount-1; i >= newCount; i--) {
      if (!resource.layers.unused[i])
        throw runtime_error("Bad texture layer count");
    }
    resource.layers.unused.resize(newCount);
    resource.layers.remaining -= oldCount - newCount;
    resource.layers.current %= newCount;
  }

  return true;
}
