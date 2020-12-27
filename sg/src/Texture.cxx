//
// SG
// Texture.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/Except.h"

#include "yf/cg/Device.h"

#include "TextureImpl.h"

using namespace SG_NS;
using namespace std;

Texture::Texture(FileType fileType, const wstring& textureFile) {
  Data data;

  switch (fileType) {
  case Internal:
    // TODO
    throw runtime_error("Mesh::Internal unimplemented");
  case Png:
    // TODO
    throw runtime_error("Mesh::Png unimplemented");
  case Bmp:
    // TODO
    throw runtime_error("Mesh::Bmp unimplemented");
  default:
    throw invalid_argument("Invalid Texture file type");
  }

  impl_ = make_unique<Impl>(data);
}

Texture::~Texture() { }

Texture::Texture(const Data& data) : impl_(make_unique<Impl>(data)) { }

Texture::Impl& Texture::impl() {
  return *impl_;
}

constexpr const uint32_t Layers = 16;

Texture::Impl::Resources Texture::Impl::resources_{};

Texture::Impl::Impl(const Data& data)
  : key_(data.format, data.size, data.levels, data.samples),
    layer_(UINT32_MAX) {

  auto it = resources_.find(key_);

  // Create a new image if none matches the data parameters
  if (it == resources_.end()) {
    auto& dev = CG_NS::Device::get();

    auto res = resources_.emplace(key_,
      Resource{dev.makeImage(data.format, data.size, Layers, data.levels,
                             data.samples), vector<bool>(Layers, true)});
    it = res.first;
  }

  // Find an unused layer to copy this data to
  vector<bool>& layers = it->second.layers;
  for (size_t i = 0; i < layers.size(); ++i) {
    if (layers[i]) {
      layer_ = i;
      layers[i] = false;
      break;
    }
  }

  if (layer_ == UINT32_MAX)
    // TODO: create a new image with more layers and transfer data
    throw runtime_error("Texture image resize unimplemented");

  // Copy the data
  CG_NS::Image& image = *it->second.image;
  CG_NS::Size2 size = data.size;
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data.data);
  // TODO: check if this works as expected
  for (uint32_t i = 0; i < data.levels; ++i) {
    image.write({0}, size, layer_, i, bytes);
    bytes += (image.bitsPerTexel_ >> 3) * size.width * size.height;
    size.width /= 2;
    size.height /= 2;
  }
}

// TODO
Texture::Impl::~Impl() { }

void Texture::Impl::updateImage(CG_NS::Offset2 offset, CG_NS::Size2 size,
                                uint32_t level, const void* data) {

  auto& image = *resources_.find(key_)->second.image;
  image.write(offset, size, layer_, level, data);
}

void Texture::Impl::copy(CG_NS::DcTable& dcTable, uint32_t allocation,
                         CG_NS::DcId id, uint32_t element, uint32_t level,
                         CG_NS::ImgSampler sampler) {

  auto& image = *resources_.find(key_)->second.image;
  dcTable.write(allocation, id, element, image, layer_, level, sampler);
}
