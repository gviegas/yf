//
// SG
// Node.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_NODE_H
#define YF_SG_NODE_H

#include <functional>
#include <vector>
#include <memory>

#include "yf/sg/Defs.h"

SG_NS_BEGIN

/// Node.
///
class Node {
 public:
  Node();
  virtual ~Node();

  /// Inserts a descendant node.
  ///
  void insert(Node& child);

  /// Removes itself.
  ///
  void drop();

  /// Removes all immediate descendants.
  ///
  void prune();

  /// Traverses the node graph.
  ///
  void traverse(std::function<bool (Node&)> callback, bool ignoreSelf);

  /// Traverses the node graph, unconditionally.
  ///
  void traverse(std::function<void (Node&)> callback, bool ignoreSelf);

  /// Counts the number of nodes in the graph.
  ///
  uint32_t count() const;

  /// Checks whether a node descends from another.
  ///
  bool isDescendantOf(const Node& node) const;

  /// Checks whether a node has no descendants.
  ///
  bool isLeaf() const;

  /// Checks whether a node has no ancestors.
  ///
  bool isRoot() const;

  /// Gets the immediate ancestor.
  ///
  Node* parent() const;

  /// Gets all immediate descendants.
  ///
  std::vector<Node*> children() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_NODE_H
