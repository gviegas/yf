//
// SG
// Node.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Node.h"
#include "yf/Except.h"

using namespace SG_NS;
using namespace std;

class Node::Impl {
 public:
  Impl(Node& node) : node_(node) { }

  ~Impl() {
    drop();
    prune();
  }

  void insert(Impl& child) {
    if (&child == this)
      throw invalid_argument("Attempt to insert a node into itself");

    if (child.parent_)
      child.drop();

    child.parent_ = this;
    if (child_) {
      child.nextSib_ = child_;
      child_->prevSib_ = &child;
    }
    child_ = &child;

    auto* node = this;
    do
      node->n_ += child.n_;
    while ((node = node->parent_));
  }

  void drop() {
    // TODO
  }

  void prune() {
    // TODO
  }

 private:
  Node& node_;
  Impl* parent_ = nullptr;
  Impl* child_ = nullptr;
  Impl* prevSib_ = nullptr;
  Impl* nextSib_ = nullptr;
  uint32_t n_ = 1;
};

Node::Node() : impl_(make_unique<Impl>(*this)) { }

Node::~Node() { }

void Node::insert(Node& child) {
  impl_->insert(*child.impl_);
}

void Node::drop() {
  impl_->drop();
}

void Node::prune() {
  impl_->prune();
}

void Node::traverse(function<bool (Node&)> callback, bool ignoreSelf) {
  // TODO
}

void Node::traverse(function<void (Node&)> callback, bool ignoreSelf) {
  // TODO
}

uint32_t Node::count() const {
  // TODO
}

bool Node::isDescendantOf(const Node& node) const {
  // TODO
}

bool Node::isLeaf() const {
  // TODO
}

bool Node::isRoot() const {
  // TODO
}

Node* Node::parent() const {
  // TODO
}

vector<Node*> Node::children() const {
  // TODO
}
