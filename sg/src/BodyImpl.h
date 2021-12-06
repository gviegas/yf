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

 private:
  std::vector<Sphere> spheres_{};
  std::vector<BBox> bboxes_{};
  ContactFn contactBegin_{};
  ContactFn contactEnd_{};
  bool dynamic_ = false;
  float mass_ = 1.0f;
  PhysicsFlags categoryMask_ = ~static_cast<PhysicsFlags>(0);
  PhysicsFlags contactMask_ = ~static_cast<PhysicsFlags>(0);
  PhysicsFlags collisionMask_ = ~static_cast<PhysicsFlags>(0);
  Node* node_ = nullptr;
  Vec3f localT_{};

  void pushShape(const Shape&);
  void nextStep();
  void undoStep();
  bool checkCollision(Impl&);

  friend void Body::update(const std::vector<Body*>&);
};

SG_NS_END

#endif // YF_SG_BODYIMPL_H
