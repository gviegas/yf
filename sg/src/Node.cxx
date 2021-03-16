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

  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;

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

  Node* parent() const {
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

  void setName(const wstring& name) {
    name_ = name;
  }

  const wstring& name() const {
    return name_;
  }

 private:
  Node& node_;
  Impl* parent_ = nullptr;
  Impl* child_ = nullptr;
  Impl* prevSib_ = nullptr;
  Impl* nextSib_ = nullptr;
  size_t n_ = 1;
  wstring name_{};
};

Node::Node() : impl_(make_unique<Impl>(*this)) { }

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

Node* Node::parent() const {
  return impl_->parent();
}

vector<Node*> Node::children() const {
  return impl_->children();
}

size_t Node::children(vector<Node*>& dst) const {
  return impl_->children(dst);
}

void Node::setName(const wstring& name) {
  impl_->setName(name);
}

const wstring& Node::name() const {
  return impl_->name();
}
