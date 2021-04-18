//
// SG
// Renderer.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <typeinfo>
#include <stdexcept>

#include "Renderer.h"
#include "Model.h"

using namespace SG_NS;
using namespace std;

void Renderer::render(Scene& scene, CG_NS::Target& target) {
  if (&scene == prevScene_) {
    // TODO
  } else {
    prevScene_ = &scene;
  }

  // TODO...
}
