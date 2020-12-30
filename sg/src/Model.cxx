//
// SG
// Model.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Model.h"
#include "Mesh.h"
#include "Texture.h"

using namespace SG_NS;
using namespace std;

class Model::Impl {
 public:
  Impl(Mesh* mesh, Texture* texture) : mesh_(mesh), texture_(texture) { }

  Mesh* mesh_;
  Texture* texture_;
};

Model::Model(Mesh& mesh, Texture& texture)
  : impl_(make_unique<Impl>(&mesh, &texture)) { }

Model::Model(Mesh& mesh) : impl_(make_unique<Impl>(&mesh, nullptr)) { }

Model::Model() : impl_(make_unique<Impl>(nullptr, nullptr)) { }

Model::~Model() { }

void Model::setMesh(Mesh* mesh) {
  impl_->mesh_ = mesh;
}

void Model::setTexture(Texture* texture) {
  impl_->texture_ = texture;
}

Mesh* Model::mesh() const {
  return impl_->mesh_;
}

Texture* Model::texture() const {
  return impl_->texture_;
}
