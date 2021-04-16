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

  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  ~Impl() = default;

 private:
  Pbrmr pbrmr_{};
  Normal normal_{};
  Occlusion occlusion_{};
  Emissive emissive_{};
};
