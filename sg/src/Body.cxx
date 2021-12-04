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

#include "BodyImpl.h"
#include "Node.h"

using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

/// Checks whether two spheres intersect each other.
///
bool intersect(const Sphere& sphere1, const Vec3f& t1,
               const Sphere& sphere2, const Vec3f& t2) {

  const Vec3f p1 = sphere1.t + t1;
  const Vec3f p2 = sphere2.t + t2;
  const auto dist = (p2 - p1).length();

  return dist < sphere1.radius + sphere2.radius;
}

/// Checks whether two bounding boxes intersect each other.
///
bool intersect(const BBox& bbox1, const Vec3f& t1,
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

/// Checks whether a sphere and a bounding box intersect each other.
///
bool intersect(const Sphere& sphere, const Vec3f& t1,
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

INTERNAL_NS_END

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

Body::Body(const Shape& shape) : impl_(make_unique<Impl>(shape)) { }

Body::Body(const vector<Shape*>& shapes) : impl_(make_unique<Impl>(shapes)) { }

Body::Body(const Body& other) : impl_(make_unique<Impl>(*other.impl_)) { }

Body& Body::operator=(const Body& other) {
  *impl_ = *other.impl_;
  return *this;
}

Body::~Body() { }

void Body::setDynamic(bool boolean) {
  impl_->setDynamic(boolean);
}

bool Body::dynamic() const {
  return impl_->dynamic();
}

Node* Body::node() {
  return impl_->node();
}

void Body::update(const vector<Body*>& bodies) {
  assert(none_of(bodies.begin(), bodies.end(), [](auto b) { return !b; }));

  const auto n = bodies.size();
  if (n < 2)
    return;

  for (size_t i = 0; i < n; i++) {
    Body* body1 = bodies[i];
    for (size_t j = (i + 1) % n; j != i; j = (j + 1) % n) {
      Body* body2 = bodies[j];
      if (body1->impl_->checkCollision(*body2->impl_)) {
        body1->impl_->undoStep();
        goto undone;
      }
    }
    body1->impl_->nextStep();
    undone:
      ;
  }
}

Body::Impl::Impl(const Shape& shape) {
  pushShape(shape);
}

Body::Impl::Impl(const vector<Shape*>& shapes) {
  assert(none_of(shapes.begin(), shapes.end(), [](auto s) { return !s; }));
  for (const auto& shape : shapes)
    pushShape(*shape);
}

void Body::Impl::setDynamic(bool boolean) {
  dynamic_ = boolean;
}

bool Body::Impl::dynamic() const {
  return dynamic_;
}

void Body::Impl::setMass(float kg) {
  assert(mass_ >= 0.0f);
  mass_ = kg;
}

float Body::Impl::mass() const {
  return mass_;
}

void Body::Impl::setNode(Node* node) {
  node_ = node;
  if (node_) {
    const auto& xform = node_->transform();
    localT_ = {xform[3][0], xform[3][1], xform[3][2]};
  }
}

Node* Body::Impl::node() {
  return node_;
}

void Body::Impl::pushShape(const Shape& shape) {
  const auto& id = typeid(shape);
  if (id == typeid(Sphere))
    spheres_.push_back(static_cast<const Sphere&>(shape));
  else if (id == typeid(BBox))
    bboxes_.push_back(static_cast<const BBox&>(shape));
  else
    throw invalid_argument("Unknown Shape type");
}

void Body::Impl::nextStep() {
  assert(node_);
  const auto& xform = node_->transform();
  localT_ = {xform[3][0], xform[3][1], xform[3][2]};
}

void Body::Impl::undoStep() {
  assert(node_);
  node_->transform()[3] = {localT_[0], localT_[1], localT_[2], 1.0f};
}

/// Checks intersections against another physics body.
///
bool Body::Impl::checkCollision(Impl& other) {
  assert(node_);
  assert(other.node_);

  const auto& xform = node_->transform();
  const Vec3f t{xform[3][0], xform[3][1], xform[3][2]};
  const auto& xform2 = other.node_->transform();
  const Vec3f t2{xform2[3][0], xform2[3][1], xform2[3][2]};

  for (const auto& sph : spheres_) {
    for (const auto& sph2 : other.spheres_)
      if (intersect(sph, t, sph2, t2))
        return true;
    for (const auto& bb2 : other.bboxes_)
      if (intersect(sph, t, bb2, t2))
        return true;
  }

  for (const auto& bb : bboxes_) {
    for (const auto& sph2 : other.spheres_)
      if (intersect(sph2, t2, bb, t))
        return true;
    for (const auto& bb2 : other.bboxes_)
      if (intersect(bb, t, bb2, t2))
        return true;
  }

  return false;
}
