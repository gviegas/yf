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

  Impl() { }
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  ~Impl() = default;

  const Pbrmr& pbrmr() const {
    return pbrmr_;
  }

  const Normal& normal() const {
    return normal_;
  }

  const Occlusion& occlusion() const {
    return occlusion_;
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

Material::Material() : impl_(make_unique<Impl>()) { }

Material::~Material() { }

const Material::Pbrmr& Material::pbrmr() const {
  return impl_->pbrmr();
}

const Material::Normal& Material::normal() const {
  return impl_->normal();
}

const Material::Occlusion& Material::occlusion() const {
  return impl_->occlusion();
}

const Material::Emissive& Material::emissive() const {
  return impl_->emissive();
}
