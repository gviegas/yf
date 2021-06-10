//
// SG
// Body.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "Body.h"
#include "Node.h"

using namespace SG_NS;
using namespace std;

class Body::Impl {
 public:
  enum Type {
    Sphere,
    BBox
  };

  Impl(const Vec3f& offset, float radius)
    : type_(Sphere), offset_(offset), v_{radius} { }

  Impl(const Vec3f& offset, const Vec3f& dimensions)
    : type_(BBox), offset_(offset), v_(dimensions) { }

  Node* node() {
    return node_;
  }

  void setNode(Node* node) {
    node_ = node;
  }

 private:
  Type type_;
  Vec3f offset_;
  Vec3f v_;
  Node* node_;
};

Body::Body(const Vec3f& offset, float radius)
  : impl_(make_unique<Impl>(offset, radius)) { }

Body::Body(const Vec3f& offset, const Vec3f& dimensions)
  : impl_(make_unique<Impl>(offset, dimensions)) { }

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
