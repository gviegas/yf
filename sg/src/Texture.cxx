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

// TODO
Texture::Impl::Impl(const Data& data) { }

// TODO
Texture::Impl::~Impl() { }

void Texture::Impl::updateImage(CG_NS::Offset2 offset, CG_NS::Size2 size,
                                uint32_t level, const void* data) {
  // TODO
  throw runtime_error("Unimplemented");
}

void Texture::Impl::copy(CG_NS::DcTable* dcTable, uint32_t allocation,
                         CG_NS::DcId id, uint32_t element, uint32_t level,
                         CG_NS::ImgSampler sampler) {
  // TODO
  throw runtime_error("Unimplemented");
}
