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
#include "PhysicsImpl.h"
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
  const auto prevMask = impl_->categoryMask_;
  *impl_ = *other.impl_;
  if (prevMask != impl_->categoryMask_ && impl_->physicsWorld_)
    impl_->physicsWorld_->impl().update(*this, prevMask);
  return *this;
}

Body::~Body() { }

Body::ContactFn& Body::contactBegin() {
  return impl_->contactBegin_;
}

Body::ContactFn& Body::contactEnd() {
  return impl_->contactEnd_;
}

void Body::setDynamic(bool boolean) {
  impl_->dynamic_ = boolean;
}

bool Body::dynamic() const {
  return impl_->dynamic_;
}

void Body::setMass(float kg) {
  if (kg < 0.0f)
    throw invalid_argument("Physics body mass less than zero");
  impl_->mass_ = kg;
}

float Body::mass() const {
  return impl_->mass_;
}

void Body::setRestitution(float cor) {
  if (cor < 0.0f || cor > 1.0f)
    throw invalid_argument("Physics body restitution outside the [0,1] range");
  impl_->restitution_ = cor;
}

float Body::restitution() const {
  return impl_->restitution_;
}

void Body::setFriction(float cof) {
  if (cof < 0.0f)
    throw invalid_argument("Physics body friction less than zero");
  impl_->friction_ = cof;
}

float Body::friction() const {
  return impl_->friction_;
}

void Body::setCategoryMask(PhysicsFlags mask) {
  const auto prevMask = impl_->categoryMask_;
  if (prevMask != mask) {
    impl_->categoryMask_ = mask;
    if (impl_->physicsWorld_)
      impl_->physicsWorld_->impl().update(*this, prevMask);
  }
}

PhysicsFlags Body::categoryMask() const {
  return impl_->categoryMask_;
}

void Body::setContactMask(PhysicsFlags mask) {
  impl_->contactMask_ = mask;
}

PhysicsFlags Body::contactMask() const {
  return impl_->contactMask_;
}

void Body::setCollisionMask(PhysicsFlags mask) {
  impl_->collisionMask_ = mask;
}

PhysicsFlags Body::collisionMask() const {
  return impl_->collisionMask_;
}

Node* Body::node() {
  return impl_->node_;
}

PhysicsWorld* Body::physicsWorld() {
  return impl_->physicsWorld_;
}

Body::Impl& Body::impl() {
  return *impl_;
}

Body::Impl::Impl(const Shape& shape) {
  pushShape(shape);
}

Body::Impl::Impl(const vector<Shape*>& shapes) {
  assert(none_of(shapes.begin(), shapes.end(), [](auto s) { return !s; }));
  for (const auto& shape : shapes)
    pushShape(*shape);
}

Body::Impl::Impl(const Impl& other)
  : spheres_(other.spheres_), bboxes_(other.bboxes_),
    contactBegin_(other.contactBegin_), contactEnd_(other.contactEnd_),
    dynamic_(other.dynamic_), mass_(other.mass_),
    restitution_(other.restitution_), friction_(other.friction_),
    categoryMask_(other.categoryMask_), contactMask_(other.contactMask_),
    collisionMask_(other.collisionMask_), node_{}, physicsWorld_{},
    contacts_{}, collisions_{}, position_{}, rotation_{} { }

Body::Impl& Body::Impl::operator=(const Impl& other) {
  spheres_ = other.spheres_;
  bboxes_ = other.bboxes_;
  contactBegin_ = other.contactBegin_;
  contactEnd_ = other.contactEnd_;
  dynamic_ = other.dynamic_;
  mass_ = other.mass_;
  restitution_ = other.restitution_;
  friction_ = other.friction_;
  categoryMask_ = other.categoryMask_;
  contactMask_ = other.contactMask_;
  collisionMask_ = other.collisionMask_;
  // Keep node and physics world
  contacts_.clear();
  collisions_.clear();
  // Keep position and rotation
  return *this;
}

void Body::Impl::setNode(Node* node) {
  node_ = node;
  if (node_)
    nextStep();
}

void Body::Impl::setPhysicsWorld(PhysicsWorld* world) {
  physicsWorld_ = world;
}

void Body::Impl::setPosition(const Vec3f& position) {
  position_ = position;
}

void Body::Impl::setRotation(const Qnionf& rotation) {
  rotation_ = rotation;
}

bool Body::Impl::intersect(const Body& body) const {
  assert(node_);
  assert(body.impl_->node_);

  if (body.impl_.get() == this)
    return true;

  const auto& xform = node_->transform();
  const Vec3f t{xform[3][0], xform[3][1], xform[3][2]};
  const auto& xform2 = body.impl_->node_->transform();
  const Vec3f t2{xform2[3][0], xform2[3][1], xform2[3][2]};

  for (const auto& sph : spheres_) {
    for (const auto& sph2 : body.impl_->spheres_)
      if (::intersect(sph, t, sph2, t2))
        return true;
    for (const auto& bb2 : body.impl_->bboxes_)
      if (::intersect(sph, t, bb2, t2))
        return true;
  }

  for (const auto& bb : bboxes_) {
    for (const auto& sph2 : body.impl_->spheres_)
      if (::intersect(sph2, t2, bb, t))
        return true;
    for (const auto& bb2 : body.impl_->bboxes_)
      if (::intersect(bb, t, bb2, t2))
        return true;
  }

  return false;
}

bool Body::Impl::inContact(const Body& body) const {
  for (const auto& contact : contacts_)
    if (contact == &body)
      return true;
  return false;
}

bool Body::Impl::inCollision(const Body& body) const {
  for (const auto& collision : collisions_)
    if (collision == &body)
      return true;
  return false;
}

void Body::Impl::updateContact(Body& self, Body& body, bool intersect) {
  assert(self.impl_.get() == this);

  if (&self == &body)
    return;

  if (intersect) {
    if (!inContact(body)) {
      contacts_.push_front(&body);
      if (contactBegin_)
        contactBegin_(self, body);
    }
  } else {
    auto prevIt = contacts_.before_begin();
    auto it = contacts_.begin();
    for (; it != contacts_.end(); it++, prevIt++) {
      if (*it == &body) {
        contacts_.erase_after(prevIt);
        if (contactEnd_)
          contactEnd_(self, body);
        return;
      }
    }
  }
}

void Body::Impl::updateCollision(Body& body, bool intersect) {
  if (body.impl_.get() == this)
    return;

  if (intersect) {
    if (!inCollision(body))
      collisions_.push_front(&body);
  } else {
    auto prevIt = collisions_.before_begin();
    auto it = collisions_.begin();
    for (; it != collisions_.end(); it++, prevIt++) {
      if (*it == &body) {
        collisions_.erase_after(prevIt);
        //return;
        break;
      }
    }
  }
}

void Body::Impl::resolveInteractions(Body& self) {
  assert(self.impl_.get() == this);
  assert(node_);

  // Handle physics bodies that changed category during contact
  if (!contacts_.empty()) {
    auto prevIt = contacts_.before_begin();
    auto it = contacts_.begin();
    while (it != contacts_.end()) {
      if ((*it)->impl_->categoryMask_ & contactMask_) {
        it++;
        prevIt++;
      } else {
        if (contactEnd_)
          contactEnd_(self, **it);
        it = contacts_.erase_after(prevIt);
      }
    }
  }

  if (collisions_.empty()) {
    // TODO...
    nextStep();
  } else {
    collisions_.clear();
    // TODO...
    undoStep();
  }
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
  // TODO: This should use world transform instead
  // TODO: `rotation_`
  const auto& xform = node_->transform();
  position_ = {xform[3][0], xform[3][1], xform[3][2]};
}

void Body::Impl::undoStep() {
  assert(node_);
  // TODO: This should use world transform instead
  // TODO: `rotation_`
  auto& t = node_->transform()[3];
  t[0] = position_[0];
  t[1] = position_[1];
  t[2] = position_[2];
}
