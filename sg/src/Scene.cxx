//
// SG
// Scene.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Scene.h"
#include "Camera.h"

using namespace SG_NS;
using namespace std;

// TODO

class Scene::Impl {
 public:
  Camera camera_{{20.0f, 20.0f, 20.0f}, {}, 1.0f};
  array<float, 4> color_{0.0f, 0.0f, 0.0f, 1.0f};
};

Scene::Scene() : impl_(make_unique<Impl>()) { }

Scene::~Scene() { }

Camera& Scene::camera() {
  return impl_->camera_;
}

array<float, 4>& Scene::color() {
  return impl_->color_;
}
