//
// SG
// Body.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_BODY_H
#define YF_SG_BODY_H

#include <memory>
#include <vector>
#include <functional>

#include "yf/sg/Defs.h"
#include "yf/sg/Physics.h"
#include "yf/sg/Vector.h"
#include "yf/sg/Quaternion.h"

SG_NS_BEGIN

class Node;

/// Physics Shape.
///
struct Shape {
  Vec3f t;
  Qnionf r;

  Shape(const Vec3f& t = {}, const Qnionf& r = {1.0f, {}});
  virtual ~Shape() = 0;
};

/// Sphere shape.
///
struct Sphere : Shape {
  float radius;

  Sphere(float radius, const Vec3f& t = {}, const Qnionf& r = {1.0f, {}});
};

/// Bounding box shape.
///
struct BBox : Shape {
  Vec3f extent;

  BBox(const Vec3f& extent, const Vec3f& t = {}, const Qnionf& r = {1.0f, {}});
};

/// Physics body.
///
class Body {
 public:
  using Ptr = std::unique_ptr<Body>;

  Body(const Shape& shape);
  Body(const std::vector<Shape*>& shapes);
  Body(const Body& other);
  Body& operator=(const Body& other);
  ~Body();

  using ContactFn = std::function<void (Body& self, Body& other)>;

  /// Contact callbacks.
  ///
  ContactFn& contactBegin();
  ContactFn& contactEnd();

  /// Whether or not the physics body can be moved.
  ///
  void setDynamic(bool boolean);
  bool dynamic() const;

  /// The physics body's mass, in kilograms.
  ///
  void setMass(float kg);
  float mass() const;

  /// Category mask for physics interactions.
  ///
  void setCategoryMask(PhysicsFlags mask);
  PhysicsFlags categoryMask() const;

  /// Contact mask for physics interactions.
  ///
  void setContactMask(PhysicsFlags mask);
  PhysicsFlags contactMask() const;

  /// Collision mask for physics interactions.
  ///
  void setCollisionMask(PhysicsFlags mask);
  PhysicsFlags collisionMask() const;

  /// Node linked to the physics body.
  ///
  Node* node();

  /// Current world of the physics body.
  ///
  PhysicsWorld* physicsWorld();

  [[deprecated]]
  static void update(const std::vector<Body*>& bodies);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  friend Node;
  friend PhysicsWorld;
};

SG_NS_END

#endif // YF_SG_BODY_H
