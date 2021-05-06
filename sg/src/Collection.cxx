//
// SG
// Collection.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "Collection.h"

using namespace SG_NS;
using namespace std;

class Collection::Impl {
 public:
  Impl() = default;
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  ~Impl() = default;

  vector<Scene::Ptr> scenes_{};
  vector<Node::Ptr> nodes_{};
  vector<Mesh> meshes_{};
  vector<Texture> textures_{};
  vector<Material> materials_{};
  vector<Skin> skins_{};
};

Collection::Collection() : impl_(make_unique<Impl>()) { }

Collection::~Collection() { }

vector<Scene::Ptr>& Collection::scenes() {
  return impl_->scenes_;
}

const std::vector<Scene::Ptr>& Collection::scenes() const {
  return impl_->scenes_;
}

vector<Node::Ptr>& Collection::nodes() {
  return impl_->nodes_;
}

const std::vector<Node::Ptr>& Collection::nodes() const {
  return impl_->nodes_;
}

vector<Mesh>& Collection::meshes() {
  return impl_->meshes_;
}

const std::vector<Mesh>& Collection::meshes() const {
  return impl_->meshes_;
}

vector<Texture>& Collection::textures() {
  return impl_->textures_;
}

const std::vector<Texture>& Collection::textures() const {
  return impl_->textures_;
}

vector<Material>& Collection::materials() {
  return impl_->materials_;
}

const std::vector<Material>& Collection::materials() const {
  return impl_->materials_;
}

vector<Skin>& Collection::skins() {
  return impl_->skins_;
}

const std::vector<Skin>& Collection::skins() const {
  return impl_->skins_;
}
