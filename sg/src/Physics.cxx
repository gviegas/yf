//
// SG
// Physics.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <type_traits>
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

void PhysicsWorld::Impl::applyChanges() {
  if (pendingChanges_.empty())
    return;

  auto changesIt = pendingChanges_.begin();
  auto bodiesIt = bodies_.begin();
  array<list<Body*>::iterator, CategoryN> groupsIts{};

  for (uint32_t i = 0; i < CategoryN; i++)
    groupsIts[i] = groups_[i].begin();

  // Remove `*changesIt` from physics world
  auto remove = [&] {
    bodiesIt = bodies_.erase(bodiesIt);
    const auto body = *changesIt++;

    auto categoryMask = body->categoryMask();
    uint32_t i = 0;
    static_assert(!is_signed<decltype(categoryMask)>());

    for (; categoryMask != 0; categoryMask >>= 1, i++) {
      if (categoryMask & 1) {
        while (*groupsIts[i] != body)
          groupsIts[i]++;
        groupsIts[i] = groups_[i].erase(groupsIts[i]);
      }
    }
  };

  // Add `*changesIt` to physics world
  auto add = [&] {
    bodies_.insert(bodiesIt, *changesIt);
    const auto body = *changesIt++;

    auto categoryMask = body->categoryMask();
    uint32_t i = 0;
    static_assert(!is_signed<decltype(categoryMask)>());

    for (; categoryMask != 0; categoryMask >>= 1, i++) {
      if (categoryMask & 1) {
        while (groupsIts[i] != groups_[i].end() && *groupsIts[i] < body)
          groupsIts[i]++;
        groups_[i].insert(groupsIts[i], body)++;
      }
    }
  };

  while (bodiesIt != bodies_.end()) {
    if (*changesIt > *bodiesIt) {
      bodiesIt++;
    } else {
      *changesIt == *bodiesIt ? remove() : add();
      if (changesIt == pendingChanges_.end()) {
        pendingChanges_.clear();
        return;
      }
    }
  }
  while (changesIt != pendingChanges_.end())
    add();
  pendingChanges_.clear();
}
