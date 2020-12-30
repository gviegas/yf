//
// SG
// Model.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_MODEL_H
#define YF_SG_MODEL_H

#include <memory>

#include "yf/sg/Node.h"

SG_NS_BEGIN

class Mesh;
class Texture;

/// Model.
///
class Model : public Node {
 public:
  Model(Mesh& mesh, Texture& texture);
  Model(Mesh& mesh);
  Model();
  ~Model();

  /// Setters.
  ///
  void setMesh(Mesh* mesh);
  void setTexture(Texture* texture);

  /// Getters.
  ///
  Mesh* mesh() const;
  Texture* texture() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_MODEL_H
