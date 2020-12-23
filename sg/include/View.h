//
// SG
// View.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_VIEW_H
#define YF_SG_VIEW_H

#include <functional>
#include <chrono>
#include <memory>

#include "yf/ws/Window.h"

#include "yf/sg/Defs.h"

SG_NS_BEGIN

class Scene;

/// View.
///
class View {
 public:
  View(WS_NS::Window* window);
  ~View();

  using UpdateFn = std::function<bool (std::chrono::nanoseconds elapsedTime)>;

  /// Starts a scene rendering loop.
  ///
  void loop(Scene& scene, uint32_t fps, const UpdateFn& update);

  /// Swaps scenes during a rendering loop.
  ///
  void swapScene(Scene& newScene);

  /// Renders a scene.
  ///
  void render(Scene& scene);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_VIEW_H
