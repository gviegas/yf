//
// SG
// Renderer.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_RENDERER_H
#define YF_SG_RENDERER_H

#include "yf/cg/Pass.h"

#include "Scene.h"

SG_NS_BEGIN

/// Renderer.
///
class Renderer {
 public:
  Renderer() = default;
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  ~Renderer() = default;

  /// Renders a scene on a given target.
  ///
  void render(Scene& scene, CG_NS::Target& target);

 private:
  // TODO...
  Scene* prevScene_{};
};

SG_NS_END

#endif // YF_SG_RENDERER_H
