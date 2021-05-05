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
  Impl(const Pbrmr& pbrmr, const Normal& normal, const Occlusion& occlusion,
       const Emissive& emissive)
    : pbrmr_(pbrmr), normal_(normal), occlusion_(occlusion),
      emissive_(emissive) { }

  Impl() = default;
  Impl(const Impl&) = default;
  Impl& operator=(const Impl&) = default;
  ~Impl() = default;

  Pbrmr pbrmr_{};
  Normal normal_{};
  Occlusion occlusion_{};
  Emissive emissive_{};
};

Material::Material(const Pbrmr& pbrmr, const Normal& normal,
                   const Occlusion& occlusion, const Emissive& emissive)
  : impl_(make_shared<Impl>(pbrmr, normal, occlusion, emissive)) { }

Material::Material(const Pbrmr& pbrmr) : Material(pbrmr, {}, {}, {}) { }

Material::Material() : impl_(make_shared<Impl>()) { }

Material::Material(const Material& other) : impl_(other.impl_) { }

Material& Material::operator=(const Material& other) {
  impl_ = other.impl_;
  return *this;
}

Material::~Material() { }

bool Material::operator==(const Material& other) const {
  return impl_ == other.impl_;
}

bool Material::operator!=(const Material& other) const {
  return impl_ != other.impl_;
}

size_t Material::hash() const {
  return std::hash<decltype(impl_)>()(impl_);
}

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
