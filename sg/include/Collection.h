//
// SG
// Collection.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_COLLECTION_H
#define YF_SG_COLLECTION_H

#include <memory>

#include "yf/sg/Defs.h"
#include "yf/sg/Scene.h"
#include "yf/sg/Model.h"
#include "yf/sg/Mesh.h"
#include "yf/sg/Texture.h"
#include "yf/sg/Material.h"
#include "yf/sg/Skin.h"

SG_NS_BEGIN

/// Collection.
///
class Collection {
 public:
  Collection();
  Collection(const Collection&) = delete;
  Collection& operator=(const Collection&) = delete;
  ~Collection();

  /// Scene nodes.
  ///
  std::vector<Scene>& scenes();
  const std::vector<Scene>& scenes() const;

  /// Model nodes.
  ///
  std::vector<Model>& models();
  const std::vector<Model>& models() const;

  /// Nodes.
  ///
  std::vector<Node>& nodes();
  const std::vector<Node>& nodes() const;

  /// Meshes.
  ///
  std::vector<Mesh>& meshes();
  const std::vector<Mesh>& meshes() const;

  /// Textures.
  ///
  std::vector<Texture>& textures();
  const std::vector<Texture>& textures() const;

  /// Materials.
  ///
  std::vector<Material>& materials();
  const std::vector<Material>& materials() const;

  /// Skins.
  ///
  std::vector<Skin>& skins();
  const std::vector<Skin>& skins() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_COLLECTION_H
