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

#include "yf/sg/Defs.h"
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
  Body(const Shape& shape);
  Body(const std::vector<Shape*>& shapes);
  Body(const Body& other);
  Body& operator=(const Body& other);
  ~Body();

  /// Node linked to the physics body.
  ///
  Node* node();

  [[deprecated]]
  static void update(const std::vector<Body*>& bodies);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  void setNode(Node* node);

  friend Node;
};

SG_NS_END

#endif // YF_SG_BODY_H
