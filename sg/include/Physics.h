//
// SG
// Physics.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_PHYSICS_H
#define YF_SG_PHYSICS_H

#include <memory>

#include "yf/sg/Defs.h"

SG_NS_BEGIN

/// Physics world.
///
class PhysicsWorld {
 public:
  PhysicsWorld(const PhysicsWorld& other);
  PhysicsWorld& operator=(const PhysicsWorld& other);
  ~PhysicsWorld();

  /// Enable/disable physics simulation.
  ///
  void enable();
  void disable();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  PhysicsWorld();

  friend class Scene;
  friend class View;
};

SG_NS_END

#endif // YF_SG_PHYSICS_H
