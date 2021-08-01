//
// SG
// Node.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cstddef>
#include <deque>

#include "Node.h"
#include "yf/Except.h"

using namespace SG_NS;
using namespace std;

class Node::Impl {
 public:
  Impl(Node& node) : node_(node) { }

  Impl(Node& node, const Impl& other)
    : node_(node), name_(other.name_), transform_(other.transform_) {

    if (other.child_)
      throw runtime_error("Cannot copy non-leaf node");

    if (other.parent_)
      other.parent_->insert(*this);
  }

  Impl(const Impl&) = delete;

  Impl& operator=(const Impl& other) {
    if (other.child_)
      throw runtime_error("Cannot copy non-leaf node");

    drop();
    prune();

    if (other.parent_)
      other.parent_->insert(*this);

    name_ = other.name_;
    transform_ = other.transform_;

    return *this;
  }

  ~Impl() {
    drop();
    prune();
  }

  void insert(Impl& child) {
    if (&child == this)
      throw invalid_argument("Cannot insert a node into itself");

    if (child.parent_)
      child.drop();

    child.parent_ = this;
    if (child_) {
      child.nextSib_ = child_;
      child_->prevSib_ = &child;
    }
    child_ = &child;

    auto node = this;
    do
      node->n_ += child.n_;
    while ((node = node->parent_));
  }

  void drop() {
    if (!parent_)
      return;

    if (nextSib_)
      nextSib_->prevSib_ = prevSib_;
    if (prevSib_)
      prevSib_->nextSib_ = nextSib_;
    else
      parent_->child_ = nextSib_;

    auto node = parent_;
    do
      node->n_ -= n_;
    while ((node = node->parent_));

    parent_ = prevSib_ = nextSib_ = nullptr;
  }

  void prune() {
    if (!child_)
      return;

    auto node = child_;
    size_t n = 0;
    for (;;) {
      n += node->n_;
      node->parent_ = nullptr;
      node = node->nextSib_;
      if (!node)
        break;
      node->prevSib_->nextSib_ = nullptr;
      node->prevSib_ = nullptr;
    }
    child_ = nullptr;

    node = this;
    do
      node->n_ -= n;
    while ((node = node->parent_));
  }

  void traverse(const function<bool (Node&)>& callback, bool ignoreSelf) {
    if (!ignoreSelf && !callback(node_))
      return;

    deque<Impl*> nodes{this};
    Impl* node;
    do {
      node = nodes.front()->child_;

      while (node) {
        if (!callback(node->node_))
          return;
        if (node->child_)
          nodes.push_back(node);
        node = node->nextSib_;
      }

      nodes.pop_front();
    } while (!nodes.empty());
  }

  void traverse(const function<void (Node&)>& callback, bool ignoreSelf) {
    if (!ignoreSelf)
      callback(node_);

    deque<Impl*> nodes{this};
    Impl* node;
    do {
      node = nodes.front()->child_;

      while (node) {
        callback(node->node_);
        if (node->child_)
          nodes.push_back(node);
        node = node->nextSib_;
      }

      nodes.pop_front();
    } while (!nodes.empty());
  }

  size_t count() const {
    return n_;
  }

  bool isDescendantOf(const Impl& node) const {
    if (!parent_)
      return false;

    auto ancestor = parent_;
    do {
      if (ancestor == &node)
        return true;
    } while ((ancestor = ancestor->parent_));

    return false;
  }

  bool isLeaf() const {
    return !child_;
  }

  bool isRoot() const {
    return !parent_;
  }

  Node* parent() {
    if (!parent_)
      return nullptr;

    return &parent_->node_;
  }

  const Node* parent() const {
    if (!parent_)
      return nullptr;

    return &parent_->node_;
  }

  vector<Node*> children() const {
    if (!child_)
      return {};

    vector<Node*> nodes;
    auto node = child_;
    do
      nodes.push_back(&node->node_);
    while ((node = node->nextSib_));

    return nodes;
  }

  size_t children(vector<Node*>& dst) {
    if (!child_)
      return 0;

    auto node = child_;
    size_t n = 0;
    do {
      dst.push_back(&node->node_);
      ++n;
    } while ((node = node->nextSib_));

    return n;
  }

  wstring& name() {
    return name_;
  }

  const wstring& name() const {
    return name_;
  }

  Mat4f& transform() {
    changed_ = true;
    return transform_;
  }

  const Mat4f& transform() const {
    return transform_;
  }

  void setT(const Vec3f& t) {
    t_ = t;
    changed_ = true;
  }

  void setR(const Qnionf& r) {
    r_ = r;
    changed_ = true;
  }

  void setS(const Vec3f& s) {
    s_ = s;
    changed_ = true;
  }

  Mat4f& worldTransform() {
    return worldXform_;
  }

  const Mat4f& worldTransform() const {
    return worldXform_;
  }

  Mat4f& worldInverse() {
    return worldInv_;
  }

  const Mat4f& worldInverse() const {
    return worldInv_;
  }

  Mat4f& worldNormal() {
    return worldNorm_;
  }

  const Mat4f& worldNormal() const {
    return worldNorm_;
  }

 private:
  Node& node_;
  Impl* parent_ = nullptr;
  Impl* child_ = nullptr;
  Impl* prevSib_ = nullptr;
  Impl* nextSib_ = nullptr;
  size_t n_ = 1;
  wstring name_{};
  bool changed_ = false;
  Mat4f transform_ = Mat4f::identity();
  Vec3f t_{};
  Qnionf r_{1.0f, {}};
  Vec3f s_{1.0f, 1.0f, 1.0f};
  Mat4f worldXform_ = Mat4f::identity();
  Mat4f worldInv_ = Mat4f::identity();
  Mat4f worldNorm_ = Mat4f::identity();
};

Node::Node() : impl_(make_unique<Impl>(*this)) { }

Node::Node(const Node& other)
  : impl_(make_unique<Impl>(*this, *other.impl_)) { }

Node& Node::operator=(const Node& other) {
  *impl_ = *other.impl_;
  return *this;
}

Node::~Node() { }

void Node::insert(Node& child) {
  impl_->insert(*child.impl_);
}

void Node::insert(const vector<Node*>& children) {
  for (const auto& node : children)
    impl_->insert(*node->impl_);
}

void Node::drop() {
  impl_->drop();
}

void Node::prune() {
  impl_->prune();
}

void Node::traverse(const function<bool (Node&)>& callback, bool ignoreSelf) {
  impl_->traverse(callback, ignoreSelf);
}

void Node::traverse(const function<void (Node&)>& callback, bool ignoreSelf) {
  impl_->traverse(callback, ignoreSelf);
}

size_t Node::count() const {
  return impl_->count();
}

bool Node::isDescendantOf(const Node& node) const {
  return impl_->isDescendantOf(*node.impl_);
}

bool Node::isLeaf() const {
  return impl_->isLeaf();
}

bool Node::isRoot() const {
  return impl_->isRoot();
}

Node* Node::parent() {
  return impl_->parent();
}

const Node* Node::parent() const {
  return impl_->parent();
}

vector<Node*> Node::children() const {
  return impl_->children();
}

size_t Node::children(vector<Node*>& dst) const {
  return impl_->children(dst);
}

wstring& Node::name() {
  return impl_->name();
}

const wstring& Node::name() const {
  return impl_->name();
}

Mat4f& Node::transform() {
  return impl_->transform();
}

const Mat4f& Node::transform() const {
  return impl_->transform();
}

void Node::setT(const Vec3f& t) {
  impl_->setT(t);
}

void Node::setR(const Qnionf& r) {
  impl_->setR(r);
}

void Node::setS(const Vec3f& s) {
  impl_->setS(s);
}

Mat4f& Node::worldTransform() {
  return impl_->worldTransform();
}

const Mat4f& Node::worldTransform() const {
  return impl_->worldTransform();
}

Mat4f& Node::worldInverse() {
  return impl_->worldInverse();
}

const Mat4f& Node::worldInverse() const {
  return impl_->worldInverse();
}

Mat4f& Node::worldNormal() {
  return impl_->worldNormal();
}

const Mat4f& Node::worldNormal() const {
  return impl_->worldNormal();
}
