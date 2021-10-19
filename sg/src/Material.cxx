//
// SG
// Material.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "Material.h"

using namespace SG_NS;
using namespace std;

class Material::Impl {
 public:
  Impl() = default;
  ~Impl() = default;

  Impl(const Impl& other)
    : pbrmr_{other.pbrmr_.colorTex
              ? make_unique<Texture>(*other.pbrmr_.colorTex)
              : nullptr,
             other.pbrmr_.colorFac,
             other.pbrmr_.metalRoughTex
              ? make_unique<Texture>(*other.pbrmr_.metalRoughTex)
              : nullptr,
             other.pbrmr_.metallic,
             other.pbrmr_.roughness},

      normal_{other.normal_.texture
               ? make_unique<Texture>(*other.normal_.texture)
               : nullptr,
              other.normal_.scale},

      occlusion_{other.occlusion_.texture
                  ? make_unique<Texture>(*other.occlusion_.texture)
                  : nullptr,
                 other.occlusion_.strength},

      emissive_{other.emissive_.texture
                 ? make_unique<Texture>(*other.emissive_.texture)
                 : nullptr,
                other.emissive_.factor},

      alphaMode_(other.alphaMode_) { }

  Impl& operator=(const Impl& other) {
    const pair<Texture::Ptr*, const Texture::Ptr*> texs[5] = {
      make_pair(&pbrmr_.colorTex, &other.pbrmr_.colorTex),
      make_pair(&pbrmr_.metalRoughTex, &other.pbrmr_.metalRoughTex),
      make_pair(&normal_.texture, &other.normal_.texture),
      make_pair(&occlusion_.texture, &other.occlusion_.texture),
      make_pair(&emissive_.texture, &other.emissive_.texture)
    };

    for (auto& tx : texs) {
      if (*tx.second)
        *tx.first = make_unique<Texture>(**tx.second);
      else
        tx.first->release();
    }

    pbrmr_.colorFac = other.pbrmr_.colorFac;
    pbrmr_.metallic = other.pbrmr_.metallic;
    pbrmr_.roughness = other.pbrmr_.roughness;
    normal_.scale = other.normal_.scale;
    occlusion_.strength = other.occlusion_.strength;
    emissive_.factor = other.emissive_.factor;
    alphaMode_ = other.alphaMode_;

    return *this;
  }

  Pbrmr pbrmr_{};
  Normal normal_{};
  Occlusion occlusion_{};
  Emissive emissive_{};
  AlphaMode alphaMode_ = Opaque;
};

Material::Material() : impl_(make_unique<Impl>()) { }

Material::Material(const Material& other)
  : impl_(make_unique<Impl>(*other.impl_)) { }

Material& Material::operator=(const Material& other) {
  *impl_ = *other.impl_;
  return *this;
}

Material::~Material() { }

Material::Pbrmr& Material::pbrmr() {
  return impl_->pbrmr_;
}

const Material::Pbrmr& Material::pbrmr() const {
  return impl_->pbrmr_;
}

Material::Normal& Material::normal() {
  return impl_->normal_;
}

const Material::Normal& Material::normal() const {
  return impl_->normal_;
}

Material::Occlusion& Material::occlusion() {
  return impl_->occlusion_;
}

const Material::Occlusion& Material::occlusion() const {
  return impl_->occlusion_;
}

Material::Emissive& Material::emissive() {
  return impl_->emissive_;
}

const Material::Emissive& Material::emissive() const {
  return impl_->emissive_;
}

void Material::setAlphaMode(AlphaMode alphaMode) {
  impl_->alphaMode_ = alphaMode;
}

Material::AlphaMode Material::alphaMode() const {
  return impl_->alphaMode_;
}
