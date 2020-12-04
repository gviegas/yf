//
// SG
// View.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "View.h"
#include "Scene.h"
#include "yf/cg/Device.h"

using namespace SG_NS;
using namespace std;

class View::Impl {
 public:
  Impl(WS_NS::Window* window) : wsi_(CG_NS::Device::get().makeWsi(window)) { }

 private:
  CG_NS::Wsi::Ptr wsi_;
};

View::View(WS_NS::Window* window) : impl_(make_unique<Impl>(window)) { }

View::~View() {
  // TODO
}

void View::render(Scene& scene) {
  // TODO
}

void View::start(Scene& scene, uint32_t fps, const UpdateFn& callback) {
  // TODO
}

void View::stop() {
  // TODO
}
