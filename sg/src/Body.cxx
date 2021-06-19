//
// SG
// Body.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <cfloat>
#include <algorithm>
#include <typeinfo>
#include <cassert>
#include <stdexcept>

#include "Body.h"
#include "Node.h"

using namespace SG_NS;
using namespace std;

//
// Shape
//

Shape::Shape(const Vec3f& t, const Qnionf& r) : t(t), r(r) { }

Shape::~Shape() { }

Sphere::Sphere(float radius, const Vec3f& t, const Qnionf& r)
  : Shape{t, r}, radius(max(radius, FLT_MIN)) { }

BBox::BBox(const Vec3f& extent, const Vec3f& t, const Qnionf& r)
  : Shape{t, r}, extent(extent) { }

//
// Body
//

class Body::Impl {
 public:
  Impl(const Shape& shape) {
    const auto& type = typeid(shape);
    if (type == typeid(Sphere))
      spheres_.push_back(static_cast<const Sphere&>(shape));
    else if (type == typeid(BBox))
      bboxes_.push_back(static_cast<const BBox&>(shape));
    else
      throw invalid_argument("Body() unknown shape type");
  }

  Impl(const vector<Shape*>& shapes) {
    for (const auto& shape : shapes) {
      assert(shape);
      const auto& type = typeid(*shape);
      if (type == typeid(Sphere))
        spheres_.push_back(static_cast<const Sphere&>(*shape));
      else if (type == typeid(BBox))
        bboxes_.push_back(static_cast<const BBox&>(*shape));
      else
        throw invalid_argument("Body() unknown shape type");
    }
  }

  Node* node() {
    return node_;
  }

  void setNode(Node* node) {
    node_ = node;
  }

 private:
  vector<Sphere> spheres_{};
  vector<BBox> bboxes_{};
  Node* node_ = nullptr;
};

Body::Body(const Shape& shape) : impl_(make_unique<Impl>(shape)) { }

Body::Body(const vector<Shape*>& shapes) : impl_(make_unique<Impl>(shapes)) { }

Body::Body(const Body& other) : impl_(make_unique<Impl>(*other.impl_)) { }

Body& Body::operator=(const Body& other) {
  *impl_ = *other.impl_;
  return *this;
}

Body::~Body() { }

Node* Body::node() {
  return impl_->node();
}

void Body::setNode(Node* node) {
  impl_->setNode(node);
}
