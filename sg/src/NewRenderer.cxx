//
// SG
// NewRenderer.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <typeinfo>
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
    // TODO
    abort();
  }

  scene_ = &scene;
  pass_ = &target.pass();

  viewport_.width = target.size().width;
  viewport_.height = target.size().height;
  scissor_.size = target.size();

  processGraph();
  allocateTables();

  // TODO...
}

void NewRenderer::processGraph() {
  drawableNodes_.clear();
  blendDrawables_.clear();
  opaqueDrawables_.clear();

  if (scene_->isLeaf())
    return;

  scene_->worldTransform() = scene_->transform();
  scene_->worldInverse() = invert(scene_->worldTransform());

  const auto& mdlId = typeid(Model);

  scene_->traverse([&](Node& node) {
    node.worldTransform() = node.parent()->worldTransform() * node.transform();
    node.worldInverse() = invert(node.worldTransform());
    node.worldNormal() = transpose(node.worldInverse());

    const auto& id = typeid(node);
    if (id == mdlId) {
      auto& mdl = static_cast<Model&>(node);

      if (!mdl.mesh())
        throw runtime_error("Cannot render models with no mesh set");

      pushDrawables(node, *mdl.mesh(), mdl.skin());
    }
  }, true);
}

void NewRenderer::pushDrawables(Node& node, Mesh& mesh, Skin* skin) {
  const size_t nodeIndex = drawableNodes_.size();
  drawableNodes_.push_back(&node);

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
      blendDrawables_.push_back({nodeIndex, mesh[i], mask, UINT32_MAX});
      drawable = &blendDrawables_.back();

    } else {
      if (material->alphaMode() == Material::Mask)
        mask |= RAlphaMask;
      // Opaque alpha mode otherwise

      opaqueDrawables_.push_back({nodeIndex, mesh[i], mask, UINT32_MAX});
      drawable = &opaqueDrawables_.back();
    }

    if (!setState(*drawable))
      // TODO
      throw runtime_error("Could not set state for Drawable)");
  }
}

template<class T>
pair<uint32_t, bool> NewRenderer::getIndex(DrawableReqMask mask,
                                           const vector<T>& container) {
  if (mask == 0 || container.size() == 0)
    return {0, false};

  uint32_t beg = 0;
  uint32_t end = container.size() - 1;
  uint32_t cur = end >> 1;

  while (beg < end) {
    if (container[cur].mask < mask)
      beg = cur + 1;
    else if (container[cur].mask > mask)
      end = cur - 1;
    else
      return {cur, true};
    cur = (beg + end) >> 1;
  }

  if (container[cur].mask < mask)
    return {cur + 1, false};
  if (container[cur].mask > mask)
    return {cur, false};
  return {cur, true};
}

bool NewRenderer::setState(Drawable& drawable) {
  const auto stateIndex = getIndex(drawable.mask, states_);

  if (!stateIndex.second) {
    CG_NS::GrState::Config config;
    config.pass = pass_;

    uint32_t vertIndex, fragIndex, tableIndex;
    if (!setShaders(drawable.mask, config, vertIndex, fragIndex) ||
        !setTables(drawable.mask, config, tableIndex))
      return false;

    setInputs(drawable.mask, config);
    config.topology = drawable.primitive.topology();
    config.polyMode = CG_NS::PolyModeFill;
    config.cullMode = drawable.mask & RAlphaBlend ?
                      CG_NS::CullModeNone : CG_NS::CullModeBack;
    config.winding = CG_NS::WindingCounterCw;

    try {
      states_.insert(states_.begin() + stateIndex.first,
                     {CG_NS::device().state(config), 0, drawable.mask,
                      vertIndex, fragIndex, tableIndex});
    } catch (...) {
      return false;
    }
  }

  drawable.stateIndex = stateIndex.first;
  states_[stateIndex.first].count++;

  return true;
}

bool NewRenderer::setShaders(DrawableReqMask mask,
                             CG_NS::GrState::Config& config,
                             uint32_t& vertShaderIndex,
                             uint32_t& fragShaderIndex) {
  mask = mask & RShaderMask;

  const auto vertIndex = getIndex(mask, vertShaders_);
  const auto fragIndex = getIndex(mask, fragShaders_);

  auto shaderPath = [&](const char* format) {
    const auto n = snprintf(nullptr, 0, format, mask);
    if (n <= 0)
      throw runtime_error("Could not create shader path string");
    string str;
    str.resize(n + 1);
    snprintf(str.data(), str.size(), format, mask);
    return str;
  };

  if (!vertIndex.second) {
    try {
      const auto str = shaderPath("%X.vert.bin");
      vertShaders_.insert(vertShaders_.begin() + vertIndex.first,
                          {CG_NS::device().shader(CG_NS::StageVertex, str),
                           0, mask});
    } catch (...) {
      return false;
    }
  }

  if (!fragIndex.second) {
    try {
      const auto str = shaderPath("%X.frag.bin");
      vertShaders_.insert(fragShaders_.begin() + fragIndex.first,
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
  vertShaderIndex = vertIndex.first;
  fragShaderIndex = fragIndex.first;
  vertShader.count++;
  fragShader.count++;

  return true;
}

bool NewRenderer::setTables(DrawableReqMask mask,
                            CG_NS::GrState::Config& config,
                            uint32_t& tableIndex) {
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

  auto& table = tables_[index.first];
  config.dcTables.push_back(table.table.get());
  tableIndex = index.first;
  table.count++;

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

bool NewRenderer::renderDrawable(Drawable& drawable,
                                 CG_NS::GrEncoder& encoder,
                                 uint64_t& offset) {

  auto& state = states_[drawable.stateIndex];
  auto& table = tables_[state.tableIndex];

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

  encoder.setState(state.state.get());
  encoder.setDcTable(1, allocation);
  drawable.primitive.impl().encodeBindings(encoder);
  drawable.primitive.impl().encodeDraw(encoder, 0, 1);

  if (drawable.mask & RAlphaBlend)
    // TODO: Improve this
    encoder.synchronize();

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
  light.l[0].notUsed = 1;

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
  const auto& node = *drawableNodes_[drawable.nodeIndex];
  const auto& m = node.worldTransform();
  const auto& v = scene_->camera().view();
  const auto mv = v * m;
  const auto& norm = node.worldNormal();
  memcpy(inst.i[0].m, m.data(), sizeof inst.i[0].m);
  memcpy(inst.i[0].mv, mv.data(), sizeof inst.i[0].mv);
  memcpy(inst.i[0].norm, norm.data(), sizeof inst.i[0].norm);
  copyInstanceSkin(inst.i[0], drawable);

  auto& table = *tables_[states_[drawable.stateIndex].tableIndex].table;
  const uint64_t size = sizeof inst;
  unifBuffer_->write(offset, size, &inst);
  table.write(allocation, InstanceUnif.id, 0, *unifBuffer_, offset, size);
  offset += size + instanceWithSkinPad_;
}

void NewRenderer::copyInstanceSkin(PerInstanceWithSkin& instance,
                                   Drawable& drawable) {
  auto& node = *drawableNodes_[drawable.nodeIndex];

  // TODO: Change this if other node types with skin are added
  assert(typeid(node) == typeid(Model));
  auto& skin = *static_cast<Model&>(node).skin();

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
  const auto& node = *drawableNodes_[drawable.nodeIndex];
  const auto& m = node.worldTransform();
  const auto& v = scene_->camera().view();
  const auto mv = v * m;
  const auto& norm = node.worldNormal();
  memcpy(inst.i[0].m, m.data(), sizeof inst.i[0].m);
  memcpy(inst.i[0].mv, mv.data(), sizeof inst.i[0].mv);
  memcpy(inst.i[0].norm, norm.data(), sizeof inst.i[0].norm);

  auto& table = *tables_[states_[drawable.stateIndex].tableIndex].table;
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

  auto& table = *tables_[states_[drawable.stateIndex].tableIndex].table;
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

  auto& table = *tables_[states_[drawable.stateIndex].tableIndex].table;
  const uint64_t size = sizeof unlit;
  unifBuffer_->write(offset, size, &unlit);
  table.write(allocation, MaterialUnif.id, 0, *unifBuffer_, offset, size);
  offset += size + materialUnlitPad_;
}

void NewRenderer::writeTextureMaps(Drawable& drawable, uint32_t allocation) {
  const auto& material = *drawable.primitive.material();
  auto& table = *tables_[states_[drawable.stateIndex].tableIndex].table;

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
