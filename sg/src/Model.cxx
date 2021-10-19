//
// SG
// Model.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Model.h"
#include "Mesh.h"
#include "Skin.h"
#include "Material.h"

using namespace SG_NS;
using namespace std;

class Model::Impl {
 public:
  Impl(Mesh* mesh, Skin* skin) : mesh_(mesh), skin_(skin) { }
  Impl() = default;

  Mesh* mesh_{};
  Skin* skin_{};
};

Model::Model(Mesh& mesh, Skin& skin)
  : impl_(make_unique<Impl>(&mesh, &skin)) { }

Model::Model(Mesh& mesh) : impl_(make_unique<Impl>(&mesh, nullptr)) { }

Model::Model() : impl_(make_unique<Impl>()) { }

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

Mesh* Model::mesh() {
  return impl_->mesh_;
}

void Model::setSkin(Skin* skin) {
  impl_->skin_ = skin;
}

Skin* Model::skin() {
  return impl_->skin_;
}
