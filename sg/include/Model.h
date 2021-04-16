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

/// Model.
///
class Model : public Node {
 public:
  Model(Mesh& mesh, Material& material);
  Model(Mesh& mesh);
  Model();
  ~Model();

  /// Setters.
  ///
  void setMesh(Mesh* mesh);
  void setMaterial(Material* material);

  /// Getters.
  ///
  Mesh* mesh() const;
  Material* material() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_MODEL_H
