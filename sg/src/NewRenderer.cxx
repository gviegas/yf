//
// SG
// NewRenderer.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <typeinfo>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cassert>

#include "yf/cg/Device.h"

#include "NewRenderer.h"
#include "Node.h"
#include "Scene.h"
#include "Camera.h"
#include "Model.h"
#include "MeshImpl.h"
#include "Skin.h"
#include "Material.h"
#include "TextureImpl.h"

using namespace SG_NS;
using namespace std;

// TODO
constexpr char ShaderPath[] = "bin/";

constexpr uint64_t UnifBufferSize = 1 << 21;
constexpr CG_NS::DcEntry GlobalUnif{0, CG_NS::DcTypeUniform, 1};
constexpr CG_NS::DcEntry LightUnif{1, CG_NS::DcTypeUniform, 1};
constexpr CG_NS::DcEntry InstanceUnif{0, CG_NS::DcTypeUniform, 1};
constexpr CG_NS::DcEntry MaterialUnif{1, CG_NS::DcTypeUniform, 1};
constexpr CG_NS::DcId FirstImgSampler = MaterialUnif.id + 1;

NewRenderer::NewRenderer() {
  auto& dev = CG_NS::device();

  cmdBuffer_ = dev.defaultQueue().cmdBuffer();
  unifBuffer_ = dev.buffer(UnifBufferSize);

  // This table will contain data common to all drawables
  mainTable_ = dev.dcTable({GlobalUnif, LightUnif});
  mainTable_->allocate(1);

  // Uniforms have device-imposed alignment requirements
  const uint64_t alignedOff = dev.limits().minDcUniformWriteAlignedOffset;
  if (alignedOff != 0) {
    uint64_t mod;
    if ((mod = sizeof(Global) % alignedOff))
      globalPad_ = alignedOff - mod;
    if ((mod = sizeof(Light) % alignedOff))
      lightPad_ = alignedOff - mod;
    if ((mod = sizeof(InstanceWithSkin) % alignedOff))
      instanceWithSkinPad_ = alignedOff - mod;
    if ((mod = sizeof(InstanceNoSkin) % alignedOff))
      instanceNoSkinPad_ = alignedOff - mod;
    if ((mod = sizeof(MaterialPbr) % alignedOff))
      materialPbrPad_ = alignedOff - mod;
    if ((mod = sizeof(MaterialUnlit) % alignedOff))
      materialUnlitPad_ = alignedOff - mod;
  }
}

void NewRenderer::render(Scene& scene, CG_NS::Target& target) {
  if (&scene == scene_) {
    // TODO
  }

  if (&target.pass() != pass_) {
    assert(target.colors());
    assert(target.depthStencil());

    const CG_NS::LoadStoreOp clear{CG_NS::LoadOpClear, CG_NS::StoreOpStore};
    onceOp_.colorOps.resize(target.colors()->size(), clear);
    onceOp_.colorValues.resize(target.colors()->size(), scene.color());
    onceOp_.depthOp = clear;
    onceOp_.depthValue = 1.0f;
    onceOp_.stencilOp = clear;
    onceOp_.stencilValue = 0xFF;

    const CG_NS::LoadStoreOp load{CG_NS::LoadOpLoad, CG_NS::StoreOpStore};
    againOp_.colorOps.resize(target.colors()->size(), load);
    againOp_.depthOp = load;
    againOp_.stencilOp = load;

    // XXX: This can be avoided when passes are `compatible`
    states_.clear();
  } else {
    fill(onceOp_.colorValues.begin(), onceOp_.colorValues.end(),
         scene.color());
  }

  scene_ = &scene;
  pass_ = &target.pass();

  viewport_.width = target.size().width;
  viewport_.height = target.size().height;
  scissor_.size = target.size();

  processGraph();
  allocateTables();

  print();

  bool done = renderOnce(target);
  while (!done)
    done = renderAgain(target);

  print();
}

void NewRenderer::processGraph() {
  opaqueDrawables_.clear();
  blendDrawables_.clear();

  if (scene_->isLeaf())
    return;

  scene_->worldTransform() = scene_->transform();
  scene_->worldInverse() = invert(scene_->worldTransform());

  auto processNode = [&](Node& node) {
    node.worldTransform() = node.parent()->worldTransform() * node.transform();
    node.worldInverse() = invert(node.worldTransform());
    node.worldNormal() = transpose(node.worldInverse());

    const auto& id = typeid(node);
    const auto& mdlId = typeid(Model);
    if (id == mdlId) {
      auto& mdl = static_cast<Model&>(node);
      if (!mdl.mesh())
        throw runtime_error("Cannot render models with no mesh set");
      pushDrawables(node, *mdl.mesh(), mdl.skin());
    }
  };

  scene_->traverse(processNode, true);
}

void NewRenderer::pushDrawables(Node& node, Mesh& mesh, Skin* skin) {
  for (size_t i = 0; i < mesh.primitiveCount(); i++) {
    const auto topology = mesh[i].topology();
    const auto dataMask = mesh[i].dataMask();
    const auto material = mesh[i].material();
    DrawableReqMask mask = 0;

    switch (topology) {
    case CG_NS::TopologyTriangle:
      break;
    case CG_NS::TopologyLine:
      mask |= RLine;
      break;
    case CG_NS::TopologyPoint:
      mask |= RPoint;
      break;
    case CG_NS::TopologyTriStrip:
      mask |= RTriStrip;
      break;
    case CG_NS::TopologyLnStrip:
      mask |= RLnStrip;
      break;
    case CG_NS::TopologyTriFan:
      mask |= RTriFan;
      break;
    }

    if (dataMask & VxDataNormal)
      mask |= RNormal;
    if (dataMask & VxDataTangent)
      mask |= RTangent;
    if (dataMask & VxDataTexCoord0)
      mask |= RTexCoord0;
    if (dataMask & VxDataTexCoord1)
      mask |= RTexCoord1;
    if (dataMask & VxDataColor0)
      mask |= RColor0;

    if (dataMask & VxDataJoints0) {
      if (!(dataMask & VxDataWeights0))
        throw runtime_error("Primitive has joint data but no weight data");
      if (!skin)
        throw runtime_error("Primitive has skinning data but no skin set");

      mask |= RSkin0;

    } else if (dataMask & VxDataWeights0) {
      throw runtime_error("Primitive has weight data but no joint data");
    }

    if (!material)
      throw runtime_error("Cannot render primitives with no material set");

    // TODO: PBRSG and Unlit materials

    if (material->pbrmr().colorTex)
      mask |= RColorMap;
    if (material->pbrmr().metalRoughTex)
      mask |= RPbrMap;
    if (material->normal().texture)
      mask |= RNormalMap;
    if (material->occlusion().texture)
      mask |= ROcclusionMap;
    if (material->emissive().texture)
      mask |= REmissiveMap;

    Drawable* drawable;
    if (material->alphaMode() == Material::Blend) {
      mask |= RAlphaBlend;

      // TODO: Sort
      blendDrawables_.push_back({node, mesh[i], mask});
      drawable = &blendDrawables_.back();

    } else {
      if (material->alphaMode() == Material::Mask)
        mask |= RAlphaMask;
      // Opaque alpha mode otherwise

      opaqueDrawables_.push_back({node, mesh[i], mask});
      drawable = &opaqueDrawables_.back();
    }

    if (!setState(*drawable))
      // TODO
      throw runtime_error("Could not set state for Drawable");
  }
}

bool NewRenderer::setState(Drawable& drawable) {
  const auto mask = drawable.mask & RStateMask;
  const auto stateIndex = getIndex(mask, states_);

  if (!stateIndex.second) {
    CG_NS::GrState::Config config;
    config.pass = pass_;

    if (!setShaders(drawable.mask, config) ||
        !setTables(drawable.mask, config))
      return false;

    setInputs(drawable.mask, config);
    config.topology = drawable.primitive.topology();
    config.polyMode = CG_NS::PolyModeFill;
    // FIXME: This should depend on material's doubleSided() instead
    config.cullMode = drawable.mask & RAlphaBlend ? CG_NS::CullModeNone :
                                                    CG_NS::CullModeBack;
    config.winding = CG_NS::WindingCounterCw;

    try {
      states_.insert(states_.begin() + stateIndex.first,
                     {CG_NS::device().state(config), 0, mask});
    } catch (...) {
      return false;
    }
  }

  auto& state = states_[stateIndex.first];
  state.count++;
  getVertShader(drawable.mask).count++;
  getFragShader(drawable.mask).count++;
  getTable(drawable.mask).count++;

  return true;
}

bool NewRenderer::setShaders(DrawableReqMask mask,
                             CG_NS::GrState::Config& config) {
  mask = mask & RShaderMask;
  const auto vertIndex = getIndex(mask, vertShaders_);
  const auto fragIndex = getIndex(mask, fragShaders_);

  auto shaderPath = [&](const char* format) {
    const auto n = snprintf(nullptr, 0, format, ShaderPath, mask);
    if (n <= 0)
      throw runtime_error("Could not create shader path string");
    string str;
    str.resize(n + 1);
    snprintf(str.data(), str.size(), format, ShaderPath, mask);
    return str;
  };

  if (!vertIndex.second) {
    try {
      const auto str = shaderPath("%s%X.vert.bin");
      vertShaders_.insert(vertShaders_.begin() + vertIndex.first,
                          {CG_NS::device().shader(CG_NS::StageVertex, str),
                           0, mask});
    } catch (...) {
      return false;
    }
  }

  if (!fragIndex.second) {
    try {
      const auto str = shaderPath("%s%X.frag.bin");
      fragShaders_.insert(fragShaders_.begin() + fragIndex.first,
                          {CG_NS::device().shader(CG_NS::StageFragment, str),
                           0, mask});
    } catch (...) {
      return false;
    }
  }

  auto& vertShader = vertShaders_[vertIndex.first];
  auto& fragShader = fragShaders_[fragIndex.first];
  config.shaders.push_back(vertShader.shader.get());
  config.shaders.push_back(fragShader.shader.get());

  return true;
}

bool NewRenderer::setTables(DrawableReqMask mask,
                            CG_NS::GrState::Config& config) {
  mask = mask & RTableMask;
  const auto index = getIndex(mask, tables_);

  if (!index.second) {
    vector<CG_NS::DcEntry> entries{InstanceUnif, MaterialUnif};

    CG_NS::DcId id = FirstImgSampler;
    auto imgSampler = [&] {
      return CG_NS::DcEntry{id++, CG_NS::DcTypeImgSampler, 1};
    };

    if (mask & RColorMap)
      entries.push_back(imgSampler());

    if (!(mask & RUnlit)) {
      // PBRMR or PBRSG
      if (mask & RPbrMap)
        entries.push_back(imgSampler());
      if (mask & RNormalMap)
        entries.push_back(imgSampler());
      if (mask & ROcclusionMap)
        entries.push_back(imgSampler());
      if (mask & REmissiveMap)
        entries.push_back(imgSampler());
    }

    uint64_t unifSize;
    if (mask & RSkin0)
      unifSize = sizeof(InstanceWithSkin) + instanceWithSkinPad_;
    else
      unifSize = sizeof(InstanceNoSkin) + instanceNoSkinPad_;
    if (mask & RUnlit)
      unifSize += sizeof(MaterialUnlit) + materialUnlitPad_;
    else
      unifSize += sizeof(MaterialPbr) + materialPbrPad_;

    try {
      tables_.insert(tables_.begin() + index.first,
                     {CG_NS::device().dcTable(entries), 0, mask, unifSize, 0});
    } catch (...) {
      return false;
    }
  }

  config.dcTables.push_back(mainTable_.get());
  config.dcTables.push_back(tables_[index.first].table.get());

  return true;
}

void NewRenderer::setInputs(DrawableReqMask mask,
                            CG_NS::GrState::Config& config) {

  config.vxInputs.push_back(vxInputFor(VxDataPosition));
  if (mask & RNormal)
    config.vxInputs.push_back(vxInputFor(VxDataNormal));
  if (mask & RTangent)
    config.vxInputs.push_back(vxInputFor(VxDataTangent));
  if (mask & RTexCoord0)
    config.vxInputs.push_back(vxInputFor(VxDataTexCoord0));
  if (mask & RTexCoord1)
    config.vxInputs.push_back(vxInputFor(VxDataTexCoord1));
  if (mask & RColor0)
    config.vxInputs.push_back(vxInputFor(VxDataColor0));
  if (mask & RSkin0) {
    config.vxInputs.push_back(vxInputFor(VxDataJoints0));
    config.vxInputs.push_back(vxInputFor(VxDataWeights0));
  }
}

template<class T>
pair<uint32_t, bool> NewRenderer::getIndex(DrawableReqMask mask,
                                           const vector<T>& container) {
  if (container.size() == 0)
    return {0, false};

  uint32_t beg = 0;
  uint32_t end = container.size();
  uint32_t mid;

  while (beg < end) {
    mid = (beg + end) >> 1;
    if (container[mid].mask == mask)
      return {mid, true};
    if (container[mid].mask < mask)
      beg = mid + 1;
    else
      end = mid;
  }

  return {(container[mid].mask > mask) ? (mid) : (mid + 1), false};
}

NewRenderer::State& NewRenderer::getState(DrawableReqMask mask) {
  assert(getIndex(mask & RStateMask, states_).second);
  return states_[getIndex(mask & RStateMask, states_).first];
}

NewRenderer::Shader& NewRenderer::getVertShader(DrawableReqMask mask) {
  assert(getIndex(mask & RShaderMask, vertShaders_).second);
  return vertShaders_[getIndex(mask & RShaderMask, vertShaders_).first];
}

NewRenderer::Shader& NewRenderer::getFragShader(DrawableReqMask mask) {
  assert(getIndex(mask & RShaderMask, fragShaders_).second);
  return fragShaders_[getIndex(mask & RShaderMask, fragShaders_).first];
}

NewRenderer::Table& NewRenderer::getTable(DrawableReqMask mask) {
  assert(getIndex(mask & RTableMask, tables_).second);
  return tables_[getIndex(mask & RTableMask, tables_).first];
}

void NewRenderer::allocateTables() {
  auto size = mainUnifSize();
  bool failed = false;

  for (auto& table : tables_) {
    try {
      table.table->allocate(table.count);
      table.remaining = table.count;
      size += unifSize(table);
    } catch (...) {
      failed = true;
      break;
    }
  }

  if (failed || !checkUnifBuffer(size))
    // Try with fewer allocations
    allocateTablesSubset();
}

void NewRenderer::allocateTablesSubset() {
  uint32_t minimum = 0;

  for (auto& table : tables_) {
    if (table.count == 0)
      table.table->allocate(0);
    else
      minimum++;
    table.remaining = table.count;
  }

  while (true) {
    auto size = mainUnifSize();
    bool failed = false;
    uint32_t limit = 0;

    for (auto& table : tables_) {
      if (table.count == 0)
        continue;
      size += unifSize(table);
      if (table.remaining == 1) {
        limit++;
        if (table.table->allocations() == 1)
          continue;
      }
      try {
        table.table->allocate(table.remaining);
      } catch (...) {
        failed = true;
      }
    }

    if (failed || !checkUnifBuffer(size)) {
      if (limit == minimum)
        throw runtime_error("Cannot allocate required tables");
      for (auto& table : tables_) {
        if (table.remaining > 1)
          table.remaining = max(1U, table.remaining >> 1);
      }
      continue;
    }
    return;
  }
}

uint64_t NewRenderer::mainUnifSize() {
  const uint64_t global = sizeof(Global) + globalPad_;
  const uint64_t light = sizeof(Light) + lightPad_;
  return global + light;
}

uint64_t NewRenderer::unifSize(const Table& table) {
  return table.unifSize * table.remaining;
}

bool NewRenderer::checkUnifBuffer(uint64_t requiredSize) {
  const uint64_t size = unifBuffer_->size();
  uint64_t newSize;

  if (requiredSize > size) {
    newSize = (requiredSize & ~(UnifBufferSize - 1)) + UnifBufferSize;
    newSize = max(requiredSize, newSize);
  } else if (requiredSize < size) {
    if (requiredSize <= UnifBufferSize)
      newSize = UnifBufferSize;
    else
      newSize = (requiredSize & ~(UnifBufferSize - 1)) + UnifBufferSize;
    if (newSize > (size >> 1))
      return true;
  } else {
    return true;
  }

  try {
    unifBuffer_.reset();
    unifBuffer_ = CG_NS::device().buffer(newSize);
  } catch (...) {
    return false;
  }
  return true;
}

bool NewRenderer::renderOnce(CG_NS::Target& target) {
  CG_NS::GrEncoder encoder;
  uint64_t offset = 0;

  encoder.setViewport(viewport_);
  encoder.setScissor(scissor_);
  encoder.setTarget(target, onceOp_);
  writeGlobal(offset);
  writeLight(offset);
  encoder.setDcTable(0, 0);

  bool check;
  if (!renderOpaqueDrawables(encoder, offset) ||
      !renderBlendDrawables(encoder, offset))
    check = false;
  else
    check = true;

  cmdBuffer_->encode(encoder);
  cmdBuffer_->enqueue();
  cmdBuffer_->queue().submit();
  return check;
}

bool NewRenderer::renderAgain(CG_NS::Target& target) {
  CG_NS::GrEncoder encoder;
  uint64_t offset = mainUnifSize();

  willRenderAgain();

  encoder.setViewport(viewport_);
  encoder.setScissor(scissor_);
  encoder.setTarget(target, againOp_);
  encoder.setDcTable(0, 0);

  bool check;
  if (!renderOpaqueDrawables(encoder, offset) ||
      !renderBlendDrawables(encoder, offset))
    check = false;
  else
    check = true;

  cmdBuffer_->encode(encoder);
  cmdBuffer_->enqueue();
  cmdBuffer_->queue().submit();
  return check;
}

bool NewRenderer::renderOpaqueDrawables(CG_NS::GrEncoder& encoder,
                                        uint64_t& offset) {
  auto n = opaqueDrawables_.size();
  while (n-- != 0) {
    auto& drawable = opaqueDrawables_.front();
    if (!renderDrawable(drawable, encoder, offset))
      opaqueDrawables_.push_back(drawable);
    opaqueDrawables_.pop_front();
  }
  return opaqueDrawables_.size() == 0;
}

bool NewRenderer::renderBlendDrawables(CG_NS::GrEncoder& encoder,
                                       uint64_t& offset) {
  while (blendDrawables_.size() != 0) {
    if (renderDrawable(blendDrawables_.front(), encoder, offset))
      blendDrawables_.pop_front();
    else
      return false;
  }
  return true;
}

bool NewRenderer::renderDrawable(Drawable& drawable,
                                 CG_NS::GrEncoder& encoder,
                                 uint64_t& offset) {
  auto& state = getState(drawable.mask);
  auto& table = getTable(drawable.mask);

  if (table.remaining == 0)
    // Out of resources
    return false;

  const auto allocation = --table.remaining;

  if (drawable.mask & RSkin0)
    writeInstanceWithSkin(offset, drawable, allocation);
  else
    writeInstanceNoSkin(offset, drawable, allocation);
  if (drawable.mask & RUnlit)
    writeMaterialUnlit(offset, drawable, allocation);
  else
    writeMaterialPbr(offset, drawable, allocation);
  writeTextureMaps(drawable, allocation);

  if (drawable.mask & RAlphaBlend)
    // TODO: Improve this
    encoder.synchronize();

  encoder.setState(*state.state);
  encoder.setDcTable(1, allocation);
  drawable.primitive.impl().encodeBindings(encoder);
  drawable.primitive.impl().encodeDraw(encoder, 0, 1);

  didRenderDrawable(drawable);
  return true;
}

void NewRenderer::writeGlobal(uint64_t& offset) {
  Global global;
  const auto& cam = scene_->camera();
  memcpy(global.v, cam.view().data(), sizeof global.v);
  memcpy(global.p, cam.projection().data(), sizeof global.p);
  memcpy(global.vp, cam.transform().data(), sizeof global.vp);
  memcpy(global.o, ortho(1.0f, 1.0f, 0.0f, -1.0f).data(), sizeof global.o);

  if (ViewportN > 1)
    // TODO
    throw runtime_error("Cannot render to multiple viewports");

  global.vport[0].x = 0.0f;
  global.vport[0].y = 0.0f;
  global.vport[0].width = viewport_.width;
  global.vport[0].height = viewport_.height;
  global.vport[0].zNear = viewport_.zNear;
  global.vport[0].zFar = viewport_.zFar;
  global.vport[0].pad1 = 0.0f;

  const uint64_t size = sizeof global;
  unifBuffer_->write(offset, size, &global);
  mainTable_->write(0, GlobalUnif.id, 0, *unifBuffer_, offset, size);
  offset += size + globalPad_;
}

void NewRenderer::writeLight(uint64_t& offset) {
  // TODO: Light nodes not implemented yet
  Light light;
  //light.l[0].notUsed = 1;

  light.l[0].notUsed = 0;
  light.l[0].lightType = 2;
  light.l[0].intensity = 20.0f;
  light.l[0].range = 50.0f;
  light.l[0].color[0] = 1.0f;
  light.l[0].color[1] = 1.0f;
  light.l[0].color[2] = 1.0f;
  light.l[0].angularScale = 0.0f;
  light.l[0].position[0] = 0.0f;
  light.l[0].position[1] = 5.0f;
  light.l[0].position[2] = 5.0f;
  light.l[0].angularOffset = 0.0f;
  light.l[0].direction[0] = -0.5774f;
  light.l[0].direction[1] = -0.5774f;
  light.l[0].direction[2] = -0.5774f;
  light.l[0].pad1 = 0.0f;
  if (LightN > 1)
    light.l[1].notUsed = 1;

  const uint64_t size = sizeof light;
  unifBuffer_->write(offset, size, &light);
  mainTable_->write(0, LightUnif.id, 0, *unifBuffer_, offset, size);
  offset += size + lightPad_;
}

void NewRenderer::writeInstanceWithSkin(uint64_t& offset, Drawable& drawable,
                                        uint32_t allocation) {
  assert(drawable.mask & RSkin0);

  // TODO
  if (InstanceN > 1)
    throw runtime_error("Cannot render multiple instances");

  InstanceWithSkin inst;
  const auto& m = drawable.node.worldTransform();
  const auto& v = scene_->camera().view();
  const auto mv = v * m;
  const auto& norm = drawable.node.worldNormal();
  memcpy(inst.i[0].m, m.data(), sizeof inst.i[0].m);
  memcpy(inst.i[0].mv, mv.data(), sizeof inst.i[0].mv);
  memcpy(inst.i[0].norm, norm.data(), sizeof inst.i[0].norm);
  copyInstanceSkin(inst.i[0], drawable);

  auto& table = *getTable(drawable.mask).table;
  const uint64_t size = sizeof inst;
  unifBuffer_->write(offset, size, &inst);
  table.write(allocation, InstanceUnif.id, 0, *unifBuffer_, offset, size);
  offset += size + instanceWithSkinPad_;
}

void NewRenderer::copyInstanceSkin(PerInstanceWithSkin& instance,
                                   Drawable& drawable) {
  // TODO: Change this if other node types with skin are added
  assert(typeid(drawable.node) == typeid(Model));
  auto& skin = *static_cast<Model&>(drawable.node).skin();

  if (skin.joints().size() > JointN)
    throw runtime_error("Cannot render drawable with this many joints");

  const size_t n = 16;
  const size_t size = sizeof(float[n]);
  uint32_t index = 0;
  float* joints = instance.joints;
  float* normJoints = instance.normJoints;

  if (skin.inverseBind().size() > 0) {
    for (const auto& joint : skin.joints()) {
      // TODO: Do this on `processGraph()` to avoid unnecessary computations
      const Mat4f jointM = joint->worldTransform() * skin.inverseBind()[index];
      memcpy(joints, jointM.data(), size);
      memcpy(normJoints, transpose(invert(jointM)).data(), size);
      joints += n;
      normJoints += n;
      index++;
    }
  } else {
    for (const auto& joint : skin.joints()) {
      memcpy(joints, joint->worldTransform().data(), size);
      memcpy(normJoints, joint->worldNormal().data(), size);
      joints += n;
      normJoints += n;
      index++;
    }
  }

#if 0
  const auto identity = Mat4f::identity();
  for (auto i = index; i < JointN; i++) {
    memcpy(joints, identity.data(), size);
    memcpy(normJoints, identity.data(), size);
    joints += n;
    normJoints += n;
  }
#endif
}

void NewRenderer::writeInstanceNoSkin(uint64_t& offset, Drawable& drawable,
                                      uint32_t allocation) {
  assert(!(drawable.mask & RSkin0));

  // TODO
  if (InstanceN > 1)
    throw runtime_error("Cannot render multiple instances");

  InstanceNoSkin inst;
  const auto& m = drawable.node.worldTransform();
  const auto& v = scene_->camera().view();
  const auto mv = v * m;
  const auto& norm = drawable.node.worldNormal();
  memcpy(inst.i[0].m, m.data(), sizeof inst.i[0].m);
  memcpy(inst.i[0].mv, mv.data(), sizeof inst.i[0].mv);
  memcpy(inst.i[0].norm, norm.data(), sizeof inst.i[0].norm);

  auto& table = *getTable(drawable.mask).table;
  const uint64_t size = sizeof inst;
  unifBuffer_->write(offset, size, &inst);
  table.write(allocation, InstanceUnif.id, 0, *unifBuffer_, offset, size);
  offset += size + instanceNoSkinPad_;
}

void NewRenderer::writeMaterialPbr(uint64_t& offset, Drawable& drawable,
                                   uint32_t allocation) {
  assert(!(drawable.mask & RUnlit));

  MaterialPbr pbr;
  const auto& material = *drawable.primitive.material();

  if (drawable.mask & RPbrsg) {
    // TODO
    throw runtime_error("Cannot render PBRSG materials");
  } else {
    const auto& pbrmr = material.pbrmr();
    memcpy(pbr.colorFac, pbrmr.colorFac.data(), sizeof pbr.colorFac);
    pbr.alphaCutoff = material.alphaCutoff();
    pbr.doubleSided = material.doubleSided();
    pbr.normalFac = material.normal().scale;
    pbr.occlusionFac= material.occlusion().strength;
    pbr.pbrFac[0] = pbrmr.metallic;
    pbr.pbrFac[1] = pbrmr.roughness;
    pbr.pbrFac[2] = pbr.pbrFac[3] = 0.0f;
    memcpy(&pbr.emissiveFac, material.emissive().factor.data(),
           sizeof pbr.emissiveFac);
    pbr.pad1 = 0.0f;
  }

  auto& table = *getTable(drawable.mask).table;
  const uint64_t size = sizeof pbr;
  unifBuffer_->write(offset, size, &pbr);
  table.write(allocation, MaterialUnif.id, 0, *unifBuffer_, offset, size);
  offset += size + materialPbrPad_;
}

void NewRenderer::writeMaterialUnlit(uint64_t& offset, Drawable& drawable,
                                     uint32_t allocation) {
  assert(drawable.mask & RUnlit);

  MaterialUnlit unlit;
  const auto& material = *drawable.primitive.material();
  // TODO: Unlit color data in 'sg::Material'
  memcpy(unlit.colorFac, Vec4f(1.0f).data(), sizeof unlit.colorFac);
  unlit.alphaCutoff = material.alphaCutoff();
  unlit.doubleSided = material.doubleSided();
  unlit.pad1 = unlit.pad2 = 0.0f;

  auto& table = *getTable(drawable.mask).table;
  const uint64_t size = sizeof unlit;
  unifBuffer_->write(offset, size, &unlit);
  table.write(allocation, MaterialUnif.id, 0, *unifBuffer_, offset, size);
  offset += size + materialUnlitPad_;
}

void NewRenderer::writeTextureMaps(Drawable& drawable, uint32_t allocation) {
  const auto& material = *drawable.primitive.material();
  auto& table = *getTable(drawable.mask).table;

  // XXX: Order must be kept in sync with `setTables()`
  CG_NS::DcId id = FirstImgSampler;
  auto copy = [&](Texture& texture) {
    texture.impl().copy(table, allocation, id++, 0, 0);
  };

  if (drawable.mask & RUnlit) {
    // TODO
    throw runtime_error("Cannot render Unlit materials");

  } else {
    if (drawable.mask & RPbrsg) {
      // TODO
      throw runtime_error("Cannot render PBRSG materials");

    } else {
      if (drawable.mask & RColorMap)
        copy(*material.pbrmr().colorTex);
      if (drawable.mask & RPbrMap)
        copy(*material.pbrmr().metalRoughTex);
    }

    if (drawable.mask & RNormalMap)
      copy(*material.normal().texture);
    if (drawable.mask & ROcclusionMap)
      copy(*material.occlusion().texture);
    if (drawable.mask & REmissiveMap)
      copy(*material.emissive().texture);
  }
}

void NewRenderer::didRenderDrawable(Drawable& drawable) {
  getState(drawable.mask).count--;
  getVertShader(drawable.mask).count--;
  getFragShader(drawable.mask).count--;
  getTable(drawable.mask).count--;
}

void NewRenderer::willRenderAgain() {
  for (auto& table : tables_) {
    if (table.count > 0)
      table.remaining = table.table->allocations();
  }
}

//
// DEVEL
//

void NewRenderer::print() const {
#ifdef YF_DEVEL
  auto printDrawable = [](const Drawable& drawable) {
    wprintf(L"   node: %p\n"
            L"   primitive: %p\n"
            L"   mask: %Xh\n",
            (void*)&drawable.node, (void*)&drawable.primitive, drawable.mask);
  };

  auto printShader = [](const Shader& shader) {
    wprintf(L"   shader: %p\n"
            L"   count: %u\n"
            L"   mask: %Xh\n",
            (void*)shader.shader.get(), shader.count, shader.mask);
  };


  auto printTable = [](const Table& table) {
    wprintf(L"   table: %p\n"
            L"   count: %u\n"
            L"   mask: %Xh\n"
            L"   unif. size: %u\n"
            L"   remaining: %u\n",
            (void*)table.table.get(), table.count, table.mask, table.unifSize,
            table.remaining);
  };

  auto printState = [](const State& state) {
    wprintf(L"   state: %p\n"
            L"   count: %u\n"
            L"   mask: %Xh\n",
            (void*)state.state.get(), state.count, state.mask);
  };

  wprintf(L"\nNewRenderer\n"
          L" unif. buffer size: %zu\n", unifBuffer_->size());
  wprintf(L" opaque drawables: #%zu\n", opaqueDrawables_.size());
  for (uint32_t i = 0; i < opaqueDrawables_.size(); i++) {
    wprintf(L"  [%u]:\n", i);
    printDrawable(opaqueDrawables_[i]);
  }
  wprintf(L" blend drawables: #%zu\n", blendDrawables_.size());
  for (uint32_t i = 0; i < blendDrawables_.size(); i++) {
    wprintf(L"  [%u]:\n", i);
    printDrawable(blendDrawables_[i]);
  }
  wprintf(L" vert. shaders: #%zu\n", vertShaders_.size());
  for (uint32_t i = 0; i < vertShaders_.size(); i++) {
    wprintf(L"  [%u]:\n", i);
    printShader(vertShaders_[i]);
  }
  wprintf(L" frag. shaders: #%zu\n", fragShaders_.size());
  for (uint32_t i = 0; i < fragShaders_.size(); i++) {
    wprintf(L"  [%u]:\n", i);
    printShader(fragShaders_[i]);
  }
  wprintf(L" tables: #%zu\n", tables_.size());
  for (uint32_t i = 0; i < tables_.size(); i++) {
    wprintf(L"  [%u]:\n", i);
    printTable(tables_[i]);
  }
  wprintf(L" states: #%zu\n", states_.size());
  for (uint32_t i = 0; i < states_.size(); i++) {
    wprintf(L"  [%u]:\n", i);
    printState(states_[i]);
  }
#endif
}
