//
// SG
// Node.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Node.h"

using namespace SG_NS;
using namespace std;

// TODO
class Node::Impl {
};

Node::Node() : impl_(make_unique<Impl>()) { }

Node::~Node() {
  // TODO
}

void Node::insert(Node& node) {
  // TODO
}

void Node::removeSelf() {
  // TODO
}

void Node::removeOthers() {
  // TODO
}

void Node::traverse(std::function<bool (Node&)> callback, bool ignoreSelf) {
  // TODO
}

void Node::traverse(std::function<void (Node&)> callback, bool ignoreSelf) {
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

Node* Node::ancestor() const {
  // TODO
}

std::vector<Node*> Node::descendants() const {
  // TODO
}
