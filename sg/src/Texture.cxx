//
// SG
// Texture.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/Except.h"

#include "TextureImpl.h"

using namespace SG_NS;
using namespace std;

// TODO
Texture::Texture(FileType fileType, const wstring& textureFile) { }

Texture::~Texture() { }

Texture::Texture(const Data& data) : impl_(make_unique<Impl>(data)) { }

Texture::Impl& Texture::impl() {
  return *impl_;
}

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
