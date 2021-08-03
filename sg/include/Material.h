//
// SG
// Material.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_MATERIAL_H
#define YF_SG_MATERIAL_H

#include <cstddef>
#include <memory>

#include "yf/sg/Defs.h"
#include "yf/sg/Vector.h"
#include "yf/sg/Texture.h"

SG_NS_BEGIN

/// Material.
///
class Material {
 public:
  using Ptr = std::unique_ptr<Material>;

  /// PBR metallic-roughness.
  ///
  struct Pbrmr {
    Texture* colorTex{};
    Vec4f colorFac{1.0f, 1.0f, 1.0f, 1.0f};
    Texture* metalRoughTex{};
    float metallic = 1.0f;
    float roughness = 1.0f;
  };

  /// Normal map.
  ///
  struct Normal {
    Texture* texture{};
    float scale = 1.0f;
  };

  /// Occlusion map.
  ///
  struct Occlusion {
    Texture* texture{};
    float strength = 1.0f;
  };

  /// Emissive map.
  ///
  struct Emissive {
    Texture* texture{};
    Vec3f factor{};
  };

  Material(const Pbrmr&, const Normal&, const Occlusion&, const Emissive&);
  Material(const Pbrmr&);
  Material();
  ~Material();

  size_t hash() const;

  /// Getters.
  ///
  Pbrmr& pbrmr();
  const Pbrmr& pbrmr() const;

  Normal& normal();
  const Normal& normal() const;

  Occlusion& occlusion();
  const Occlusion& occlusion() const;

  Emissive& emissive();
  const Emissive& emissive() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_MATERIAL_H
