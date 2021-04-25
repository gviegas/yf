//
// SG
// Renderer.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <algorithm>
#include <typeinfo>
#include <stdexcept>
#include <cassert>

#include "yf/cg/Device.h"

#include "Renderer.h"
#include "Model.h"
#include "Material.h"
#include "TextureImpl.h"
#include "MeshImpl.h"
#include "Camera.h"

using namespace SG_NS;
using namespace std;

// TODO: consider allowing custom length values
constexpr uint64_t UnifLength = 1ULL << 14;
// TODO
constexpr uint64_t GlbLength = Mat4f::dataSize() << 1;
constexpr uint64_t MdlLength = Mat4f::dataSize() << 1;

Renderer::Renderer() {
  auto& dev = CG_NS::device();

  // One global table instance for shared uniforms
  const CG_NS::DcEntries glb{{Uniform, {CG_NS::DcTypeUniform, 1}}};
  glbTable_ = dev.dcTable(glb);
  glbTable_->allocate(1);

  cmdBuffer_ = dev.defaultQueue().cmdBuffer();

  unifBuffer_ = dev.buffer(UnifLength);
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
  enc.clearColor(scene.color());
  enc.clearDepth(1.0f);

  // Update global uniform buffer
  uint64_t off = 0;
  uint64_t len;

  len = Mat4f::dataSize();
  unifBuffer_->write(off, len, scene.camera().view().data());
  off += len;
  unifBuffer_->write(off, len, scene.camera().projection().data());
  off += len;
  // TODO: other global data (light, viewport, ortho matrix, ...)

  glbTable_->write(0, Uniform, 0, *unifBuffer_, 0, off);

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

      const auto& m = mdl->transform();
      const auto mv = scene.camera().view() * m;
      const auto beg = off;
      len = Mat4f::dataSize();
      unifBuffer_->write(off, len, m.data());
      off += len;
      unifBuffer_->write(off, len, mv.data());
      off += len;
      // TODO: other instance data

      tab.write(inst, Uniform, 0, *unifBuffer_, beg, off);

      if (matl) {
        const pair<Texture*, CG_NS::DcId> texs[]{
          {matl->pbrmr().colorTex, ColorImgSampler},
          {matl->pbrmr().metalRoughTex, MetalRoughImgSampler},
          {matl->normal().texture, NormalImgSampler},
          {matl->occlusion().texture, OcclusionImgSampler},
          {matl->emissive().texture, EmissiveImgSampler}};

        for (const auto& tp : texs) {
          if (tp.first)
            tp.first->impl().copy(tab, inst, tp.second, 0, 0, nullptr);
        }
        // TODO: also copy factors to uniform buffer
      } else {
        // TODO
        throw runtime_error("Cannot render models with no material set");
      }

      if (mesh)
        mesh->impl().encode(enc, 0, 1);
      else
        // TODO
        throw runtime_error("Cannot render models with no mesh set");

      ++inst;
    }
  };

  renderMdl();

  cmdBuffer_->encode(enc);
  cmdBuffer_->enqueue();
  const_cast<CG_NS::Queue&>(cmdBuffer_->queue()).submit();
}

void Renderer::processGraph(Scene& scene) {
  models_.clear();

  if (scene.isLeaf())
    return;

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
  auto& dev = CG_NS::device();

  // Set model resources and returns required uniform space
  auto setMdl = [&](Resource& resource, uint32_t instN, uint32_t allocN) {
    assert(instN > 0);
    assert(allocN > 0);

    if (resource.shaders.empty()) {
      // TODO: select shaders based on number of instances
      for (const auto& tp : MdlShaders)
        resource.shaders.push_back(dev.shader(tp.first,
                                              wstring(ShaderDir)+tp.second));
    }

    if (!resource.table) {
      const CG_NS::DcEntries inst{
        {Uniform,              {CG_NS::DcTypeUniform,    instN}},
        {ColorImgSampler,      {CG_NS::DcTypeImgSampler, 1}},
        {MetalRoughImgSampler, {CG_NS::DcTypeImgSampler, 1}},
        {NormalImgSampler,     {CG_NS::DcTypeImgSampler, 1}},
        {OcclusionImgSampler,  {CG_NS::DcTypeImgSampler, 1}},
        {EmissiveImgSampler,   {CG_NS::DcTypeImgSampler, 1}}};
      resource.table = dev.dcTable(inst);
    }

    if (resource.table->allocations() != allocN)
      resource.table->allocate(allocN);

    if (!resource.state) {
      vector<CG_NS::Shader*> shd;
      for (const auto& s : resource.shaders)
        shd.push_back(s.get());

      const vector<CG_NS::DcTable*> tab{glbTable_.get(), resource.table.get()};

      const vector<CG_NS::VxInput> inp{vxInputFor(VxTypePosition),
                                       vxInputFor(VxTypeTangent),
                                       vxInputFor(VxTypeNormal),
                                       vxInputFor(VxTypeTexCoord0),
                                       vxInputFor(VxTypeTexCoord1),
                                       vxInputFor(VxTypeColor0),
                                       vxInputFor(VxTypeJoints0),
                                       vxInputFor(VxTypeWeights0)};

      resource.state = dev.state({prevPass_, shd, tab, inp,
                                  CG_NS::PrimitiveTriangle,
                                  CG_NS::PolyModeFill, CG_NS::CullModeBack,
                                  CG_NS::WindingCounterCw});
    }

    return MdlLength * instN * allocN;
  };

  // TODO: instanced rendering
  if (any_of(models_.begin(), models_.end(),
             [](const auto& kv) { return kv.second.size() > 1; }))
    throw runtime_error("Instanced rendering of models unimplemented");

  // TODO: compute this value on `processGraph()`
  const auto uniqMdlN = count_if(models_.begin(), models_.end(),
                                 [](const auto& kv)
                                 { return kv.second.size() == 1; });

  uint64_t unifLen = GlbLength;

  // Set models
  if (models_.empty()) {
    resource_.reset();
    // TODO: reset other resources when implemented
  } else {
    unifLen += setMdl(resource_, 1, uniqMdlN);
    // TODO: instanced draw models
  }

  unifLen = (unifLen & ~255) + 256;

  // TODO: improve resizing
  // TODO: also consider shrinking if buffer grows too much
  if (unifLen > unifBuffer_->size_)
    unifBuffer_ = dev.buffer(unifLen);
}
