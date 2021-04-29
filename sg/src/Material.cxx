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
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  ~Impl() = default;

  Pbrmr& pbrmr() {
    return pbrmr_;
  }

  const Pbrmr& pbrmr() const {
    return pbrmr_;
  }

  Normal& normal() {
    return normal_;
  }

  const Normal& normal() const {
    return normal_;
  }

  Occlusion& occlusion() {
    return occlusion_;
  }

  const Occlusion& occlusion() const {
    return occlusion_;
  }

  Emissive& emissive() {
    return emissive_;
  }

  const Emissive& emissive() const {
    return emissive_;
  }

 private:
  Pbrmr pbrmr_{};
  Normal normal_{};
  Occlusion occlusion_{};
  Emissive emissive_{};
};

Material::Material(const Pbrmr& pbrmr, const Normal& normal,
                   const Occlusion& occlusion, const Emissive& emissive)
  : impl_(make_unique<Impl>(pbrmr, normal, occlusion, emissive)) { }

Material::Material(const Pbrmr& pbrmr) : Material(pbrmr, {}, {}, {}) { }

Material::Material() : impl_(make_unique<Impl>()) { }

Material::~Material() { }

Material::Pbrmr& Material::pbrmr() {
  return impl_->pbrmr();
}

const Material::Pbrmr& Material::pbrmr() const {
  return impl_->pbrmr();
}

Material::Normal& Material::normal() {
  return impl_->normal();
}

const Material::Normal& Material::normal() const {
  return impl_->normal();
}

Material::Occlusion& Material::occlusion() {
  return impl_->occlusion();
}

const Material::Occlusion& Material::occlusion() const {
  return impl_->occlusion();
}

Material::Emissive& Material::emissive() {
  return impl_->emissive();
}

const Material::Emissive& Material::emissive() const {
  return impl_->emissive();
}
