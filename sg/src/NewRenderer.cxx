//
// SG
// NewRenderer.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <typeinfo>

#include "yf/cg/Device.h"

#include "NewRenderer.h"
#include "Node.h"
#include "Scene.h"
#include "Model.h"
#include "Mesh.h"
#include "Skin.h"
#include "Material.h"

using namespace SG_NS;
using namespace std;

constexpr uint64_t UnifBufferSize = 1 << 21;

constexpr CG_NS::DcEntry GlobalUnif{0, CG_NS::DcTypeUniform, 1};

NewRenderer::NewRenderer() {
  auto& dev = CG_NS::device();

  cmdBuffer_ = dev.defaultQueue().cmdBuffer();
  unifBuffer_ = dev.buffer(UnifBufferSize);

  // This table will contain data common to all drawables
  mainTable_ = dev.dcTable({GlobalUnif});
  mainTable_->allocate(1);
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

    if (material) {
      mask |= RMaterial;

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

      if (material->alphaMode() == Material::Blend) {
        mask |= RAlphaBlend;
        // TODO: Sort
        blendDrawables_.push_back({nodeIndex, mesh[i], mask, UINT32_MAX});
        continue;
      }

      if (material->alphaMode() == Material::Mask)
        mask |= RAlphaMask;
    }

    opaqueDrawables_.push_back({nodeIndex, mesh[i], mask, UINT32_MAX});
  }
}

void NewRenderer::processGraph(Scene& scene) {
  drawableNodes_.clear();
  blendDrawables_.clear();
  opaqueDrawables_.clear();

  if (scene.isLeaf())
    return;

  scene.worldTransform() = scene.transform();
  scene.worldInverse() = invert(scene.worldTransform());

  const auto& mdlId = typeid(Model);

  scene.traverse([&](Node& node) {
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
