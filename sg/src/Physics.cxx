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

PhysicsWorld::~PhysicsWorld() { }

void PhysicsWorld::enable() {
  impl_->enabled_ = true;
}

void PhysicsWorld::disable() {
  impl_->enabled_ = false;
}
