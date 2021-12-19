//
// SG
// Physics.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_PHYSICS_H
#define YF_SG_PHYSICS_H

#include <cstdint>
#include <memory>

#include "yf/sg/Defs.h"
#include "yf/sg/Vector.h"

SG_NS_BEGIN

/// Type used when defining physics interactions.
///
using PhysicsFlags = uint32_t;

/// Physics world.
///
class PhysicsWorld {
 public:
  /// The physics world's gravity force.
  ///
  Vec3f& gravity();
  const Vec3f& gravity() const;

  /// Enables/disables physics simulation.
  ///
  void enable();
  void disable();

  /// Checks whether physics simulation is enabled or not.
  ///
  bool isEnabled() const;

  class Impl;
  Impl& impl();

 private:
  std::unique_ptr<Impl> impl_;

  PhysicsWorld();
  PhysicsWorld(const PhysicsWorld& other);
  PhysicsWorld& operator=(const PhysicsWorld& other);
  ~PhysicsWorld();

  friend class Scene;
};

SG_NS_END

#endif // YF_SG_PHYSICS_H
