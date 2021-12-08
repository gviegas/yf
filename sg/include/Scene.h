//
// SG
// Scene.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_SG_SCENE_H
#define YF_SG_SCENE_H

#include <array>

#include "yf/sg/Node.h"

SG_NS_BEGIN

class PhysicsWorld;
class Camera;

/// Scene graph.
///
class Scene : public Node {
 public:
  using Ptr = std::unique_ptr<Scene>;

  Scene();
  Scene(const Scene& other);
  Scene& operator=(const Scene& other);
  ~Scene();

  /// XXX: Scenes must be root nodes.
  ///
  bool isInsertable() const final;

  /// The scene's physics world.
  ///
  PhysicsWorld& physicsWorld();
  const PhysicsWorld& physicsWorld() const;

  /// The scene's camera.
  ///
  Camera& camera();
  const Camera& camera() const;

  /// The scene's color.
  ///
  std::array<float, 4>& color();
  const std::array<float, 4>& color() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  void willInsert(Node&) override;
  void willDrop(Node&) override;
  void willPrune(Node&) override;
};

SG_NS_END

#endif // YF_SG_SCENE_H
