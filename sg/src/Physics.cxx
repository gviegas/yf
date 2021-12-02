//
// SG
// Physics.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "PhysicsImpl.h"

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
