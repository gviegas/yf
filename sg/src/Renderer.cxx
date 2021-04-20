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
#include "MeshImpl.h"

using namespace SG_NS;
using namespace std;

Renderer::Renderer() {
  auto& dev = CG_NS::device();

  // One global table instance for shared uniforms
  const CG_NS::DcEntries glb{{Uniform, {CG_NS::DcTypeUniform, 1}}};
  glbTable_ = dev.dcTable(glb);
  glbTable_->allocate(1);

  cmdBuffer_ = dev.defaultQueue().cmdBuffer();
}

void Renderer::render(Scene& scene, CG_NS::Target& target) {
  auto pass = &target.pass();
  if (pass != prevPass_) {
    resource_.reset();
  } else if (&scene == prevScene_) {
    // TODO
  }

  prevScene_ = &scene;
  prevPass_ = pass;

  processGraph(scene);
  prepare();

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
}

void Renderer::prepare() {
  if (models_.empty()) {
    resource_.reset();
    return;
  }

  // TODO: instanced rendering
  if (any_of(models_.begin(), models_.end(),
             [](const auto& kv) { return kv.second.size() > 1; }))
    throw runtime_error("Instanced rendering of models unimplemented");

  auto& dev = CG_NS::device();

  if (resource_.shaders.empty()) {
    for (const auto& tp : MdlShaders)
      resource_.shaders.push_back(dev.shader(tp.first,
                                             wstring(ShaderDir) + tp.second));
  }

  // TODO: compute this value on `processGraph()`
  const auto uniqMdlN = count_if(models_.begin(), models_.end(),
                                 [](const auto& kv)
                                 { return kv.second.size() == 1; });

  if (!resource_.table) {
    const CG_NS::DcEntries inst{
      {Uniform,              {CG_NS::DcTypeUniform,    1}},
      {ColorImgSampler,      {CG_NS::DcTypeImgSampler, 1}},
      {MetalRoughImgSampler, {CG_NS::DcTypeImgSampler, 1}},
      {NormalImgSampler,     {CG_NS::DcTypeImgSampler, 1}},
      {OcclusionImgSampler,  {CG_NS::DcTypeImgSampler, 1}},
      {EmissiveImgSampler,   {CG_NS::DcTypeImgSampler, 1}}};
    resource_.table = dev.dcTable(inst);
  }

  if (resource_.table->allocations() != uniqMdlN)
    resource_.table->allocate(uniqMdlN);

  if (!resource_.state) {
    vector<CG_NS::Shader*> shd;
    for (const auto& s : resource_.shaders)
      shd.push_back(s.get());

    const vector<CG_NS::DcTable*> tab{glbTable_.get(), resource_.table.get()};

    const vector<CG_NS::VxInput> inp{vxInputFor(VxTypePosition),
                                     vxInputFor(VxTypeTangent),
                                     vxInputFor(VxTypeNormal),
                                     vxInputFor(VxTypeTexCoord0),
                                     vxInputFor(VxTypeTexCoord1),
                                     vxInputFor(VxTypeColor0),
                                     vxInputFor(VxTypeJoints0),
                                     vxInputFor(VxTypeWeights0)};

    resource_.state = dev.state({prevPass_, shd, tab, inp,
                                 CG_NS::PrimitiveTriangle, CG_NS::PolyModeFill,
                                 CG_NS::CullModeBack, CG_NS::WindingCounterCw});
  }
}
