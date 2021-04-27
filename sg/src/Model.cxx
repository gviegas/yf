//
// SG
// Model.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Model.h"
#include "Mesh.h"
#include "Material.h"

using namespace SG_NS;
using namespace std;

class Model::Impl {
 public:
  Impl(Mesh* mesh, Material* material) : mesh_(mesh), material_(material) { }

  Mesh* mesh_;
  Material* material_;
};

Model::Model(Mesh& mesh, Material& material)
  : impl_(make_unique<Impl>(&mesh, &material)) { }

Model::Model(Mesh& mesh) : impl_(make_unique<Impl>(&mesh, nullptr)) { }

Model::Model() : impl_(make_unique<Impl>(nullptr, nullptr)) { }

Model::Model(const Model& other)
  : Node(other), impl_(make_unique<Impl>(*other.impl_)) { }

Model& Model::operator=(const Model& other) {
  Node::operator=(other);
  *impl_ = *other.impl_;
  return *this;
}

Model::~Model() { }

void Model::setMesh(Mesh* mesh) {
  impl_->mesh_ = mesh;
}

void Model::setMaterial(Material* material) {
  impl_->material_ = material;
}

Mesh* Model::mesh() const {
  return impl_->mesh_;
}

Material* Model::material() const {
  return impl_->material_;
}
