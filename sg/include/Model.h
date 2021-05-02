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
class Material;
class Skin;

/// Model.
///
class Model : public Node {
 public:
  Model(Mesh& mesh, Material& material, Skin& skin);
  Model();
  Model(const Model& other);
  Model& operator=(const Model& other);
  ~Model();

  /// Setters.
  ///
  void setMesh(Mesh* mesh);
  void setMaterial(Material* material);
  void setSkin(Skin* skin);

  /// Getters.
  ///
  Mesh* mesh();
  Material* material();
  Skin* skin();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_MODEL_H
