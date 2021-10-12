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
  Impl(Impl&&) = default;
  Impl& operator=(const Impl&) = delete;
  Impl& operator=(Impl&&) = default;
  ~Impl() = default;

  vector<Scene::Ptr> scenes_{};
  vector<Node::Ptr> nodes_{};
  vector<Mesh::Ptr> meshes_{};
  vector<Skin::Ptr> skins_{};
  vector<Texture::Ptr> textures_{};
  vector<Material::Ptr> materials_{};
  vector<Animation::Ptr> animations_{};
};

Collection::Collection(const string& pathname) : Collection() {
  load(pathname);
}

Collection::Collection() : impl_(make_unique<Impl>()) { }

Collection::~Collection() { }

void Collection::load(const string& pathname) {
  loadGLTF(*this, pathname);
}

void Collection::clear() {
  impl_->scenes_.clear();
  impl_->nodes_.clear();
  impl_->meshes_.clear();
  impl_->skins_.clear();
  impl_->textures_.clear();
  impl_->materials_.clear();
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

vector<Skin::Ptr>& Collection::skins() {
  return impl_->skins_;
}

const vector<Skin::Ptr>& Collection::skins() const {
  return impl_->skins_;
}


vector<Texture::Ptr>& Collection::textures() {
  return impl_->textures_;
}

const vector<Texture::Ptr>& Collection::textures() const {
  return impl_->textures_;
}

vector<Material::Ptr>& Collection::materials() {
  return impl_->materials_;
}

const vector<Material::Ptr>& Collection::materials() const {
  return impl_->materials_;
}

vector<Animation::Ptr>& Collection::animations() {
  return impl_->animations_;
}

const vector<Animation::Ptr>& Collection::animations() const {
  return impl_->animations_;
}
