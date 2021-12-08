//
// SG
// Physics.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <cassert>

#include "PhysicsImpl.h"
#include "BodyImpl.h"
#include "Scene.h"

using namespace SG_NS;
using namespace std;

PhysicsWorld::PhysicsWorld() : impl_(make_unique<Impl>()) { }

PhysicsWorld::PhysicsWorld(const PhysicsWorld& other)
  : impl_(make_unique<Impl>(*other.impl_)) { }

PhysicsWorld& PhysicsWorld::operator=(const PhysicsWorld& other) {
  impl_ = make_unique<Impl>(*other.impl_);
  return *this;
}

PhysicsWorld::~PhysicsWorld() { }

void PhysicsWorld::enable() {
  impl_->enabled_ = true;
}

void PhysicsWorld::disable() {
  impl_->enabled_ = false;
}

bool PhysicsWorld::isEnabled() const {
  return impl_->enabled_;
}

void PhysicsWorld::Impl::add(Body& body) {
  assert(find(bodies_.begin(), bodies_.end(), &body) == bodies_.end() ||
         pendingChanges_.find(&body) != pendingChanges_.end());

  auto res = pendingChanges_.insert(&body);
  if (!res.second)
    pendingChanges_.erase(res.first);
}

void PhysicsWorld::Impl::remove(Body& body) {
  assert(find(bodies_.begin(), bodies_.end(), &body) != bodies_.end() ||
         pendingChanges_.find(&body) != pendingChanges_.end());

  auto res = pendingChanges_.insert(&body);
  if (!res.second)
    pendingChanges_.erase(res.first);
}

void PhysicsWorld::Impl::clear() {
  bodies_.clear();
  for (auto& group : groups_)
    group.clear();
  pendingChanges_.clear();
}

void PhysicsWorld::Impl::evaluate(Scene& scene) {
  assert(scene.physicsWorld().impl_.get() == this);

  // FIXME: Temporary implementation
  vector<Body*> bodies;
  scene.traverse([&](Node& node) {
    if (node.body())
      bodies.push_back(node.body());
  }, true);
  Body::update(bodies);
}
