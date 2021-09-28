//
// SG
// Collection.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_COLLECTION_H
#define YF_SG_COLLECTION_H

#include <memory>
#include <string>

#include "yf/sg/Defs.h"
#include "yf/sg/Scene.h"
#include "yf/sg/Node.h"
#include "yf/sg/Mesh.h"
#include "yf/sg/Skin.h"
#include "yf/sg/Texture.h"
#include "yf/sg/Material.h"
#include "yf/sg/Animation.h"

SG_NS_BEGIN

/// Collection.
///
class Collection {
 public:
  Collection(const std::string& pathname);
  Collection();
  Collection(const Collection&) = delete;
  Collection& operator=(const Collection&) = delete;
  ~Collection();

  /// Loads collection from file.
  ///
  void load(const std::string& pathname);

  /// Clears collection contents.
  ///
  void clear();

  /// Scenes.
  ///
  std::vector<Scene::Ptr>& scenes();
  const std::vector<Scene::Ptr>& scenes() const;

  /// Nodes.
  ///
  std::vector<Node::Ptr>& nodes();
  const std::vector<Node::Ptr>& nodes() const;

  /// Meshes.
  ///
  std::vector<Mesh::Ptr>& meshes();
  const std::vector<Mesh::Ptr>& meshes() const;

  /// Skins.
  ///
  std::vector<Skin::Ptr>& skins();
  const std::vector<Skin::Ptr>& skins() const;

  /// Textures.
  ///
  std::vector<Texture::Ptr>& textures();
  const std::vector<Texture::Ptr>& textures() const;

  /// Materials.
  ///
  std::vector<Material::Ptr>& materials();
  const std::vector<Material::Ptr>& materials() const;

  /// Animations.
  ///
  std::vector<Animation::Ptr>& animations();
  const std::vector<Animation::Ptr>& animations() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_COLLECTION_H
