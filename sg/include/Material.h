//
// SG
// Material.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_MATERIAL_H
#define YF_SG_MATERIAL_H

#include <memory>

#include "yf/sg/Defs.h"
#include "yf/sg/Vector.h"

SG_NS_BEGIN

class Texture;

/// Material.
///
class Material {
 public:
  /// PBR metallic-roughness.
  ///
  struct Pbrmr {
    Texture* colorTex = nullptr;
    Vec4f colorFac{1.0f, 1.0f, 1.0f, 1.0f};
    Texture* metalRoughTex = nullptr;
    float metalness = 1.0f;
    float roughness = 1.0f;
  };

  /// Normal map.
  ///
  struct Normal {
    Texture* texture = nullptr;
    float scale = 1.0f;
  };

  /// Occlusion map.
  ///
  struct Occlusion {
    Texture* texture = nullptr;
    float strength = 1.0f;
  };

  /// Emissive map.
  ///
  struct Emissive {
    Texture* texture = nullptr;
    Vec3f factor{};
  };

  Material(const Pbrmr&, const Normal&, const Occlusion&, const Emissive&);
  Material(const Pbrmr&);
  Material();
  ~Material();

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
