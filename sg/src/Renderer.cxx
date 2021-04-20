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
#include "Material.h"
#include "TextureImpl.h"
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

  // Encode common commands
  CG_NS::GrEncoder enc;
  enc.setTarget(&target);
  enc.setViewport({0.0f, 0.0f, static_cast<float>(target.size_.width),
                   static_cast<float>(target.size_.height), 0.0f, 1.0f});
  enc.setScissor({{0}, target.size_});
  enc.setDcTable(GlbTable, 0);

  // TODO: update global uniform buffer

  // Render unique models
  auto renderMdl = [&] {
    uint32_t inst = 0;

    for (auto& kv : models_) {
      if (kv.second.size() > 1)
        continue;

      auto mdl = kv.second[0];
      auto matl = mdl->material();
      auto mesh = mdl->mesh();
      auto& tab = *resource_.table;

      enc.setState(resource_.state.get());
      enc.setDcTable(MdlTable, inst);

      // TODO: update instance's uniform buffer

      if (matl) {
        // TODO: also copy factors to uniform buffer
        if (matl->pbrmr().colorTex)
          matl->pbrmr().colorTex->impl()
            .copy(tab, inst, ColorImgSampler, 0, 0, nullptr);
        if (matl->pbrmr().metalRoughTex)
          matl->pbrmr().metalRoughTex->impl()
            .copy(tab, inst, MetalRoughImgSampler, 0, 0, nullptr);
        if (matl->normal().texture)
          matl->normal().texture->impl()
            .copy(tab, inst, NormalImgSampler, 0, 0, nullptr);
        if (matl->occlusion().texture)
          matl->occlusion().texture->impl()
            .copy(tab, inst, OcclusionImgSampler, 0, 0, nullptr);
        if (matl->emissive().texture)
          matl->emissive().texture->impl()
            .copy(tab, inst, EmissiveImgSampler, 0, 0, nullptr);
      } else {
        // TODO
        throw runtime_error("Cannot render models with no material set");
      }

      if (mesh)
        mesh->impl().encode(enc, 0, 0);
      else
        // TODO
        throw runtime_error("Cannot render models with no mesh set");

      ++inst;
    }
  };

  renderMdl();

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
