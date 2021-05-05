//
// SG
// Model.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Skin.h"

using namespace SG_NS;
using namespace std;

class Model::Impl {
 public:
  Impl(Mesh& mesh, Material& material, Skin& skin)
    : mesh_(mesh), material_(material), skin_(skin) { }

  Impl() = default;

  Mesh mesh_;
  Material material_;
  Skin skin_;
};

Model::Model(Mesh& mesh, Material& material, Skin& skin)
  : impl_(make_unique<Impl>(mesh, material, skin)) { }

Model::Model() : impl_(make_unique<Impl>()) { }

Model::Model(const Model& other)
  : Node(other), impl_(make_unique<Impl>(*other.impl_)) { }

Model& Model::operator=(const Model& other) {
  Node::operator=(other);
  *impl_ = *other.impl_;
  return *this;
}

Model::~Model() { }

void Model::setMesh(Mesh& mesh) {
  impl_->mesh_ = mesh;
}

void Model::setMaterial(Material& material) {
  impl_->material_ = material;
}

void Model::setSkin(Skin& skin) {
  impl_->skin_ = skin;
}

Mesh& Model::mesh() {
  return impl_->mesh_;
}

Material& Model::material() {
  return impl_->material_;
}

Skin& Model::skin() {
  return impl_->skin_;
}
