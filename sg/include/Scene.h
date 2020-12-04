//
// SG
// Scene.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_SCENE_H
#define YF_SG_SCENE_H

#include "yf/sg/Node.h"

SG_NS_BEGIN

/// Scene graph.
///
class Scene : public Node {
 public:
  Scene();
  ~Scene();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_SCENE_H
