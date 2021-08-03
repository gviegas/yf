//
// SG
// Model.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_SG_MODEL_H
#define YF_SG_MODEL_H

#include <memory>

#include "yf/sg/Node.h"

SG_NS_BEGIN

class Mesh;
class Skin;
class Material;

/// Model.
///
class Model : public Node {
 public:
  using Ptr = std::unique_ptr<Model>;

  Model(Mesh& mesh, Skin& skin, Material& material);
  Model(Mesh& mesh, Material& material);
  Model();
  Model(const Model& other);
  Model& operator=(const Model& other);
  ~Model();

  /// Setters.
  ///
  void setMesh(Mesh* mesh);
  void setSkin(Skin* skin);
  void setMaterial(Material* material);

  /// Getters.
  ///
  Mesh* mesh();
  Skin* skin();
  Material* material();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_MODEL_H
