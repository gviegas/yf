//
// SG
// BodyImpl.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_BODYIMPL_H
#define YF_SG_BODYIMPL_H

#include <forward_list>

#include "Body.h"

SG_NS_BEGIN

/// Physics Body implementation details.
///
class Body::Impl {
 public:
  Impl(const Shape& shape);
  Impl(const std::vector<Shape*>& shapes);
  Impl(const Impl& other);
  Impl& operator=(const Impl& other);

  void setNode(Node* node);
  void setPhysicsWorld(PhysicsWorld* world);
  void setPosition(const Vec3f& position);
  void setRotation(const Qnionf& rotation);

  /// Checks whether two physics bodies intersect each other.
  /// This check ignores interaction masks.
  ///
  bool intersect(const Body& body) const;

  /// Checks whether two physics bodies are in contact.
  ///
  bool inContact(const Body& body) const;

  /// Checks whether two physics bodies are colliding.
  ///
  bool inCollision(const Body& body) const;

  /// Updates the physics body's contact state.
  ///
  void updateContact(Body& self, Body& body, bool intersect);

  /// Updates the physics body's collision state.
  ///
  void updateCollision(Body& body, bool intersect);

  /// Resolves interactions for the physics body.
  /// Called by the physics world after all of its bodies have been updated.
  ///
  void resolveInteractions(Body& self);

 private:
  std::vector<Sphere> spheres_{};
  std::vector<BBox> bboxes_{};
  ContactFn contactBegin_{};
  ContactFn contactEnd_{};
  bool dynamic_ = true;
  float mass_ = 1.0f;
  float restitution_ = 0.5f;
  float friction_ = 0.25f;
  PhysicsFlags categoryMask_ = 1;
  PhysicsFlags contactMask_ = 0;
  PhysicsFlags collisionMask_ = ~static_cast<PhysicsFlags>(0);
  Node* node_ = nullptr;
  PhysicsWorld* physicsWorld_ = nullptr;
  std::forward_list<Body*> contacts_{};
  std::forward_list<Body*> collisions_{};
  Vec3f position_{};
  Qnionf rotation_{1.0f, {}};
  Vec3f velocity_{};
  Vec3f finalVelocity_{};
  Qnionf spin_{1.0f, {}};
  Qnionf finalSpin_{1.0f, {}};

  void pushShape(const Shape&);
  void combineVelocity(const Impl&);
  void nextStep();
  void undoStep();

  friend Body;
};

SG_NS_END

#endif // YF_SG_BODYIMPL_H
