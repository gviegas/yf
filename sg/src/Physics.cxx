//
// SG
// Physics.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <type_traits>
#include <algorithm>
#include <cassert>

#include "PhysicsImpl.h"
#include "BodyImpl.h"
#include "Node.h"

using namespace SG_NS;
using namespace std;

PhysicsWorld::PhysicsWorld() : impl_(make_unique<Impl>(*this)) { }

PhysicsWorld::PhysicsWorld(const PhysicsWorld& other)
  : impl_(make_unique<Impl>(*this, *other.impl_)) { }

PhysicsWorld& PhysicsWorld::operator=(const PhysicsWorld& other) {
  impl_ = make_unique<Impl>(*this, *other.impl_);
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

PhysicsWorld::Impl::Impl(PhysicsWorld& physicsWorld)
  : physicsWorld_(physicsWorld) { }

PhysicsWorld::Impl::Impl(PhysicsWorld& physicsWorld, const Impl& other)
  : physicsWorld_(physicsWorld), enabled_(other.enabled_) { }

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

void PhysicsWorld::Impl::update(Body& body, PhysicsFlags prevCategoryMask) {
  assert(find(bodies_.begin(), bodies_.end(), &body) != bodies_.end());

  // TODO
}

void PhysicsWorld::Impl::clear() {
  for (auto& body : bodies_)
    body->impl_->setPhysicsWorld(nullptr);
  bodies_.clear();

  for (auto& group : groups_)
    group.clear();

  for (auto& body : pendingChanges_)
    if (!body->node())
      // Was released by its Node - get rid of it
      delete body;
  pendingChanges_.clear();
}

void PhysicsWorld::Impl::evaluate() {
  print();

  applyChanges();

  print();

  // FIXME: Temporary implementation
  Body::update({bodies_.begin(), bodies_.end()});
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

    if (!body->node())
      // Was released by its Node - get rid of it
      delete body;
    else
      body->impl_->setPhysicsWorld(nullptr);
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

    body->impl_->setPhysicsWorld(&physicsWorld_);
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

//
// DEVEL
//

void PhysicsWorld::Impl::print() const {
#ifdef YF_DEVEL
  wprintf(L"\nPhysicsWorld\n");

  auto printBody = [](Body* body, const char* indent) {
    wprintf(L"%sBody %p:\n"
            L"%s Node: %p (%ls)\n",
            indent, static_cast<void*>(body),
            indent, static_cast<void*>(body->node()),
            (body->node() ? body->node()->name().data() : L""));
  };

  wprintf(L" bodies_: %zu\n", bodies_.size());
  for (const auto& body : bodies_)
    printBody(body, "  ");

  wprintf(L" groups_: %zu\n", groups_.size());
  uint32_t grpIndex = 0;
  for (const auto& group : groups_) {
    wprintf(L"  groups_[%u]: %zu\n", grpIndex, groups_[grpIndex].size());
    grpIndex++;
    for (const auto& body : group)
      printBody(body, "   ");
  }

  wprintf(L" pendingChanges_: %zu\n", pendingChanges_.size());
  for (const auto& body : pendingChanges_)
    printBody(body, "  ");
#endif
}
