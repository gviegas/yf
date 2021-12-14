//
// SG
// BodyImpl.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_BODYIMPL_H
#define YF_SG_BODYIMPL_H

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

  ContactFn& contactBegin();
  ContactFn& contactEnd();

  void setDynamic(bool boolean);
  bool dynamic() const;

  void setMass(float kg);
  float mass() const;

  void setCategoryMask(PhysicsFlags mask);
  PhysicsFlags categoryMask() const;

  void setContactMask(PhysicsFlags mask);
  PhysicsFlags contactMask() const;

  void setCollisionMask(PhysicsFlags mask);
  PhysicsFlags collisionMask() const;

  void setNode(Node* node);
  Node* node();

  void setPhysicsWorld(PhysicsWorld* world);
  PhysicsWorld* physicsWorld();

  /// Checks whether two physics bodies intersect each other.
  /// This check ignores interaction masks.
  ///
  bool intersect(Impl& other);

  [[deprecated]] static void processCollisions(const std::vector<Body*>&);

 private:
  std::vector<Sphere> spheres_{};
  std::vector<BBox> bboxes_{};
  ContactFn contactBegin_{};
  ContactFn contactEnd_{};
  bool dynamic_ = false;
  float mass_ = 1.0f;
  PhysicsFlags categoryMask_ = 1;
  PhysicsFlags contactMask_ = 0;
  PhysicsFlags collisionMask_ = ~static_cast<PhysicsFlags>(0);
  Node* node_ = nullptr;
  Vec3f localT_{};
  PhysicsWorld* physicsWorld_ = nullptr;

  void pushShape(const Shape&);
  void nextStep();
  void undoStep();
};

SG_NS_END

#endif // YF_SG_BODYIMPL_H
