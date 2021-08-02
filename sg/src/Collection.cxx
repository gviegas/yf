//
// SG
// Collection.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "Collection.h"
#include "DataGLTF.h"

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
  vector<Mesh::Ptr> meshes_{};
  vector<Texture> textures_{};
  vector<Material> materials_{};
  vector<Skin> skins_{};
  vector<Animation> animations_{};
};

Collection::Collection(const wstring& pathname) : Collection() {
  load(pathname);
}

Collection::Collection() : impl_(make_unique<Impl>()) { }

Collection::~Collection() { }

void Collection::load(const wstring& pathname) {
  loadGLTF(*this, pathname);
}

void Collection::clear() {
  impl_->scenes_.clear();
  impl_->nodes_.clear();
  impl_->meshes_.clear();
  impl_->textures_.clear();
  impl_->materials_.clear();
  impl_->skins_.clear();
  impl_->animations_.clear();
}

vector<Scene::Ptr>& Collection::scenes() {
  return impl_->scenes_;
}

const vector<Scene::Ptr>& Collection::scenes() const {
  return impl_->scenes_;
}

vector<Node::Ptr>& Collection::nodes() {
  return impl_->nodes_;
}

const vector<Node::Ptr>& Collection::nodes() const {
  return impl_->nodes_;
}

vector<Mesh::Ptr>& Collection::meshes() {
  return impl_->meshes_;
}

const vector<Mesh::Ptr>& Collection::meshes() const {
  return impl_->meshes_;
}

vector<Texture>& Collection::textures() {
  return impl_->textures_;
}

const vector<Texture>& Collection::textures() const {
  return impl_->textures_;
}

vector<Material>& Collection::materials() {
  return impl_->materials_;
}

const vector<Material>& Collection::materials() const {
  return impl_->materials_;
}

vector<Skin>& Collection::skins() {
  return impl_->skins_;
}

const vector<Skin>& Collection::skins() const {
  return impl_->skins_;
}

vector<Animation>& Collection::animations() {
  return impl_->animations_;
}

const vector<Animation>& Collection::animations() const {
  return impl_->animations_;
}
