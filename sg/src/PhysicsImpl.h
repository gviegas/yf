//
// SG
// PhysicsImpl.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_PHYSICSIMPL_H
#define YF_SG_PHYSICSIMPL_H

#include "Physics.h"

SG_NS_BEGIN

class Scene;

/// PhysicsWorld implementation details.
///
class PhysicsWorld::Impl {
 public:
  Impl() = default;
  Impl(const Impl&) = default;
  Impl& operator=(const Impl&) = default;
  ~Impl() = default;

  /// Evaluates the physics simulation.
  ///
  void evaluate(Scene& scene);

 private:
  bool enabled_ = true;

  friend PhysicsWorld;
};

SG_NS_END

#endif // YF_SG_PHYSICSIMPL_H
