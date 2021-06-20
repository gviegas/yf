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
    if (node_) {
      const auto& xform = node_->transform();
      localT_ = {xform[3][0], xform[3][1], xform[3][2]};
    }
  }

  void next() {
    const auto& xform = node_->transform();
    localT_ = {xform[3][0], xform[3][1], xform[3][2]};
  }

  void undo() {
    node_->transform()[3] = {localT_[0], localT_[1], localT_[2], 1.0f};
  }

  bool check(Impl& other) {
    const auto& xform = node_->transform();
    const Vec3f t{xform[3][0], xform[3][1], xform[3][2]};
    const auto& xform2 = other.node_->transform();
    const Vec3f t2{xform2[3][0], xform2[3][1], xform2[3][2]};

    for (const auto& sph : spheres_) {
      for (const auto& sph2 : other.spheres_)
        if (intersects(sph, t, sph2, t2))
          return true;
      for (const auto& bb2 : other.bboxes_)
        if (intersects(sph, t, bb2, t2))
          return true;
    }

    for (const auto& bb : bboxes_) {
      for (const auto& sph2 : other.spheres_)
        if (intersects(sph2, t2, bb, t))
          return true;
      for (const auto& bb2 : other.bboxes_)
        if (intersects(bb, t, bb2, t2))
          return true;
    }

    return false;
  }

  static bool intersects(const Sphere& sphere1, const Vec3f& t1,
                         const Sphere& sphere2, const Vec3f& t2) {

    const Vec3f p1 = sphere1.t + t1;
    const Vec3f p2 = sphere2.t + t2;
    const auto dist = (p2 - p1).length();

    return dist < sphere1.radius + sphere2.radius;
  }

  static bool intersects(const BBox& bbox1, const Vec3f& t1,
                         const BBox& bbox2, const Vec3f& t2) {

    const Vec3f p1 = bbox1.t + t1;
    const Vec3f off1 = bbox1.extent * 0.5f;
    const Vec3f min1 = p1 - off1;
    const Vec3f max1 = p1 + off1;

    const Vec3f p2 = bbox2.t + t2;
    const Vec3f off2 = bbox2.extent * 0.5f;
    const Vec3f min2 = p2 - off2;
    const Vec3f max2 = p2 + off2;

    return min1[0] <= max2[0] && max1[0] >= min2[0] &&
           min1[1] <= max2[1] && max1[1] >= min2[1] &&
           min1[2] <= max2[2] && max1[2] >= min2[2];
  }

  static bool intersects(const Sphere& sphere, const Vec3f& t1,
                         const BBox& bbox, const Vec3f& t2) {

    const Vec3f p1 = sphere.t + t1;

    Vec3f p2 = bbox.t + t2;
    const Vec3f off2 = bbox.extent * 0.5f;
    const Vec3f min2 = p2 - off2;
    const Vec3f max2 = p2 + off2;

    p2[0] = clamp(p1[0], min2[0], max2[0]);
    p2[1] = clamp(p1[1], min2[1], max2[1]);
    p2[2] = clamp(p1[2], min2[2], max2[2]);

    const auto dist = (p2 - p1).length();

    return dist < sphere.radius;
  }

 private:
  vector<Sphere> spheres_{};
  vector<BBox> bboxes_{};
  Node* node_ = nullptr;
  Vec3f localT_{};
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

void Body::update(const vector<Body*>& bodies) {
  if (bodies.empty())
    return;

  const auto n = bodies.size();

  for (size_t i = 0; i < n; ++i) {
    auto body1 = bodies[i];
    assert(body1);

    auto chk = false;
    for (size_t j = i+1; j < n; ++j) {
      auto body2 = bodies[j];
      assert(body2);

      chk = body1->impl_->check(*body2->impl_);
      if (chk) {
        body2->impl_->undo();
        break;
      }
    }

    if (chk)
      body1->impl_->undo();
    else
      body1->impl_->next();
  }

}
