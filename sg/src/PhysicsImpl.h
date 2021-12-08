//
// SG
// PhysicsImpl.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_PHYSICSIMPL_H
#define YF_SG_PHYSICSIMPL_H

#include <climits>
#include <array>
#include <list>

#include "Physics.h"

SG_NS_BEGIN

class Scene;
class Body;

/// PhysicsWorld implementation details.
///
class PhysicsWorld::Impl {
 public:
  Impl() = default;
  Impl(const Impl&) = default;
  Impl& operator=(const Impl&) = default;
  ~Impl() = default;

  /// Adds a new physics body to the world.
  ///
  void add(Body& body);

  /// Removes a physics body from the world.
  ///
  void remove(Body& body);

  /// Removes all physics bodies from the world.
  ///
  void clear();

  /// Evaluates the physics simulation.
  ///
  void evaluate(Scene& scene);

 private:
  bool enabled_ = true;

  /// Physics bodies will be split into groups as indicated by the bits set
  /// in their category masks.
  /// Each category bit represents a different group.
  ///
  static constexpr uint32_t CategoryN = sizeof(PhysicsFlags) * CHAR_BIT;
  std::list<Body*> bodies_{};
  std::array<std::list<Body*>, CategoryN> groups_{};

  friend PhysicsWorld;
};

SG_NS_END

#endif // YF_SG_PHYSICSIMPL_H
