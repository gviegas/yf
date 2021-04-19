//
// SG
// Renderer.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <algorithm>
#include <typeinfo>
#include <stdexcept>

#include "yf/cg/Device.h"

#include "Renderer.h"
#include "Model.h"

using namespace SG_NS;
using namespace std;

Renderer::Renderer() {
  auto& dev = CG_NS::device();

  // One global table instance for shared uniforms
  const CG_NS::DcEntries glb{{Uniform, {CG_NS::DcTypeUniform, 1}}};
  glbTable_ = dev.dcTable(glb);
  glbTable_->allocate(1);
}

void Renderer::render(Scene& scene, CG_NS::Target& target) {
  if (&scene == prevScene_) {
    // TODO
  } else {
    prevScene_ = &scene;
  }

  processGraph(scene);

  // TODO...
}

void Renderer::processGraph(Scene& scene) {
  if (scene.isLeaf())
    return;

  models_.clear();

  scene.traverse([&](Node& node) {
    if (typeid(node) == typeid(Model)) {
      auto& mdl = static_cast<Model&>(node);
      const MdlKey key{mdl.mesh(), mdl.material()};

      auto it = models_.find(key);
      if (it == models_.end())
        models_.emplace(key, MdlValue{&mdl});
      else
        it->second.push_back(&mdl);
    }
  }, true);

  // TODO: instanced rendering
  if (models_.empty()) {
    mdlShader_.reset();
    mdlTable_.reset();
    mdlState_.reset();
  } else {
    // XXX
    if (any_of(models_.begin(), models_.end(),
               [](const auto& kv) { return kv.second.size() > 1; }))
      throw runtime_error("Instanced rendering of models unimplemented");

    // TODO...
  }
}
