//
// SG
// View.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <thread>
#include <stdexcept>

#include "View.h"
#include "Scene.h"
#include "yf/cg/Device.h"

using namespace SG_NS;
using namespace std;

class View::Impl {
 public:
  Impl(WS_NS::Window* window) : wsi_(CG_NS::Device::get().makeWsi(window)) { }

  void loop(Scene& scene, uint32_t fps, const UpdateFn& update) {
    if (fps == 0)
      throw invalid_argument("View loop() `fps` must be greater than zero");

    looping_ = true;

    const chrono::nanoseconds ipd{1'000'000'000 / fps};
    auto before = chrono::system_clock::now();
    auto now = before;

    while (update(now-before)) {
      render(scene);

      before = now;
      now = chrono::system_clock::now();

      if (now-before < ipd) {
        this_thread::sleep_until(now + ipd-(now-before));
        now = chrono::system_clock::now();
      }
    }

    looping_ = false;
  }

  void render(Scene& scene) {
    // TODO
  }

 private:
  CG_NS::Wsi::Ptr wsi_;
  bool looping_ = false;
};

View::View(WS_NS::Window* window) : impl_(make_unique<Impl>(window)) { }

View::~View() { }

void View::loop(Scene& scene, uint32_t fps, const UpdateFn& update) {
  impl_->loop(scene, fps, update);
}

void View::render(Scene& scene) {
  impl_->render(scene);
}
