//
// SG
// Body.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_BODY_H
#define YF_SG_BODY_H

#include <memory>

#include "yf/sg/Defs.h"
#include "yf/sg/Vector.h"
#include "yf/sg/Quaternion.h"

SG_NS_BEGIN

class Node;

/// Physics Shape.
///
struct Shape {
  const Vec3f t;
  const Qnionf r;
};

/// Sphere shape.
///
struct Sphere : Shape {
  const float radius;

  Sphere(float radius, const Vec3f& t = {}, const Qnionf& r = {1.0f, {}});
};

/// Bounding box shape.
///
struct BBox : Shape {
  const Vec3f extent;

  BBox(const Vec3f& extent, const Vec3f& t = {}, const Qnionf& r = {1.0f, {}});
};

/// Physics body.
///
class Body {
 public:
  Body(const Vec3f& offset, float radius);
  Body(const Vec3f& offset, const Vec3f& dimensions);
  Body(const Body& other);
  Body& operator=(const Body& other);
  ~Body();

  Node* node();
  void setNode(Node* node);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_BODY_H
