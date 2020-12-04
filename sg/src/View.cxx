//
// SG
// View.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <thread>

#include "View.h"
#include "Scene.h"
#include "yf/cg/Device.h"

using namespace SG_NS;
using namespace std;

class View::Impl {
 public:
  Impl(WS_NS::Window* window) : wsi_(CG_NS::Device::get().makeWsi(window)) { }

  void render(Scene& scene) {
    // TODO
  }

  void start(Scene& scene, uint32_t fps, const UpdateFn& callback) {
    if (started_)
      stop();
    started_ = true;

    const chrono::nanoseconds ipd(1'000'000'000 / fps);
    auto before = chrono::system_clock::now();
    auto now = before;

    while (started_) {
      auto dt = now - before;
      callback(dt);
      render(scene);
      before = now;
      now = chrono::system_clock::now();
      if (now - before < ipd) {
        this_thread::sleep_until(now + ipd-(now-before));
        now = chrono::system_clock::now();
      }
    }
  }

  void stop() {
    // TODO
  }

 private:
  CG_NS::Wsi::Ptr wsi_;
  bool started_ = false;
};

View::View(WS_NS::Window* window) : impl_(make_unique<Impl>(window)) { }

View::~View() { }

void View::render(Scene& scene) {
  impl_->render(scene);
}

void View::start(Scene& scene, uint32_t fps, const UpdateFn& callback) {
  impl_->start(scene, fps, callback);
}

void View::stop() {
  impl_->stop();
}
