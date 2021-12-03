//
// SG
// BodyImpl.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_BODYIMPL_H
#define YF_SG_BODYIMPL_H

#include <vector>

#include "Body.h"

SG_NS_BEGIN

/// Physics Body implementation details.
///
class Body::Impl {
 public:
  Impl(const Shape& shape);
  Impl(const std::vector<Shape*>& shapes);

  void setNode(Node* node);
  Node* node();

 private:
  std::vector<Sphere> spheres_{};
  std::vector<BBox> bboxes_{};
  Node* node_ = nullptr;
  Vec3f localT_{};

  void pushShape(const Shape& shape);
  void next();
  void undo();
  bool check(Impl& other);

  friend void Body::update(const std::vector<Body*>&);
};

SG_NS_END

#endif // YF_SG_BODYIMPL_H
