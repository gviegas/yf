//
// SG
// Scene.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Scene.h"
#include "PhysicsImpl.h"
#include "Camera.h"

using namespace SG_NS;
using namespace std;

class Scene::Impl {
 public:
  Impl() = default;
  Impl(const Impl& other) = default;
  Impl& operator=(const Impl&) = default;
  ~Impl() = default;

  PhysicsWorld physicsWorld_{};
  Camera camera_{{20.0f, 20.0f, 20.0f}, {}, 1.0f};
  array<float, 4> color_{0.0f, 0.0f, 0.0f, 1.0f};
};

Scene::Scene() : impl_(make_unique<Impl>()) { }

Scene::Scene(const Scene& other)
  : Node(other), impl_(make_unique<Impl>(*other.impl_)) { }

Scene& Scene::operator=(const Scene& other) {
  Node::operator=(other);
  *impl_ = *other.impl_;
  return *this;
}

Scene::~Scene() { }

bool Scene::isInsertable() const {
  return false;
}

PhysicsWorld& Scene::physicsWorld() {
  return impl_->physicsWorld_;
}

const PhysicsWorld& Scene::physicsWorld() const {
  return impl_->physicsWorld_;
}

Camera& Scene::camera() {
  return impl_->camera_;
}

const Camera& Scene::camera() const {
  return impl_->camera_;
}

array<float, 4>& Scene::color() {
  return impl_->color_;
}

const array<float, 4>& Scene::color() const {
  return impl_->color_;
}

void Scene::willInsert(Node& node) {
  Body* body = node.body();
  if (body && !node.isDescendantOf(*this))
    impl_->physicsWorld_.impl_->add(*body);
}

void Scene::willDrop(Node& node) {
  node.traverse([&](Node& node) {
    Body* body = node.body();
    if (body)
      impl_->physicsWorld_.impl_->remove(*body);
  }, false);
}
