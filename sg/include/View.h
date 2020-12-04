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

  /// Renders a scene.
  ///
  void render(Scene& scene);

  /// Starts a rendering loop.
  ///
  using UpdateFn = std::function<void (std::chrono::nanoseconds)>;
  void start(Scene& scene, uint32_t fps, const UpdateFn& callback);

  /// Stops the current rendering loop.
  ///
  void stop();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_VIEW_H
