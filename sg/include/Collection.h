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
#include "yf/sg/Texture.h"
#include "yf/sg/Material.h"
#include "yf/sg/Skin.h"
#include "yf/sg/Animation.h"

SG_NS_BEGIN

/// Collection.
///
class Collection {
 public:
  Collection(const std::wstring& pathname);
  Collection();
  Collection(const Collection&) = delete;
  Collection& operator=(const Collection&) = delete;
  ~Collection();

  /// Loads collection from file.
  ///
  void load(const std::wstring& pathname);

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

  /// Animations.
  ///
  std::vector<Animation>& animations();
  const std::vector<Animation>& animations() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_COLLECTION_H
