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

class Camera;

/// Scene graph.
///
class Scene : public Node {
 public:
  Scene();
  Scene(const Scene& other);
  Scene& operator=(const Scene& other);
  ~Scene();

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
};

SG_NS_END

#endif // YF_SG_SCENE_H
