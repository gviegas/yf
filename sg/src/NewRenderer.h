//
// SG
// NewRenderer.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_NEWRENDERER_H
#define YF_SG_NEWRENDERER_H

#include <cstddef>
#include <vector>

#include "yf/cg/Pass.h"
#include "yf/cg/Queue.h"
#include "yf/cg/Buffer.h"
#include "yf/cg/DcTable.h"
#include "yf/cg/State.h"

#include "Defs.h"

SG_NS_BEGIN

class Scene;
class Node;
class Mesh;
class Primitive;
class Skin;

/// New renderer.
///
class NewRenderer {
 public:
  NewRenderer();
  NewRenderer(const NewRenderer&) = delete;
  NewRenderer& operator=(const NewRenderer&) = delete;
  ~NewRenderer() = default;

  /// Renders a scene on a given target.
  ///
  void render(Scene& scene, CG_NS::Target& target);

 private:
  CG_NS::CmdBuffer::Ptr cmdBuffer_{};
  CG_NS::Buffer::Ptr unifBuffer_{};
  CG_NS::DcTable::Ptr mainTable_{};

  CG_NS::Pass* prevPass_{};
  Scene* prevScene_{};

  enum DrawableReq {
    // Has `Material`
    RMaterial = 1 << 0,

    // Has given texture maps
    RColorMap     = 1 << 4,
    RPbrMap       = 1 << 5,
    RNormalMap    = 1 << 6,
    ROcclusionMap = 1 << 7,
    REmissiveMap  = 1 << 8,

    // Table req. mask
    RTableMask = 0xFFF,

    // Which alpha mode (default is opaque)
    RAlphaBlend = 1 << 12,
    RAlphaMask  = 1 << 13,

    // Has given vertex attributes
    RNormal    = 1 << 14,
    RTangent   = 1 << 15,
    RTexCoord0 = 1 << 16,
    RTexCoord1 = 1 << 17,
    RColor0    = 1 << 18,

    // Has `Skin`, `Joints0` and `Weights0`
    RSkin0 = 1 << 19,

    // Shader req. mask
    RShaderMask = 0xFFFFFF,

    // Which topology (default is triangle)
    RPoint    = 1 << 24,
    RLine     = 1 << 25,
    RLnStrip  = 1 << 26,
    RTriStrip = 1 << 27,
    RTriFan   = 1 << 28,

    // State req. mask
    RStateMask = 0xFFFFFFFF
  };

  using DrawableReqMask = uint32_t;

  struct Drawable {
    size_t nodeIndex;
    Primitive& primitive;
    DrawableReqMask mask;
    uint32_t stateIndex;
  };

  struct Shader {
    CG_NS::Shader::Ptr shader;
    uint32_t count;
    DrawableReqMask mask;
  };

  struct Table {
    CG_NS::DcTable::Ptr table;
    uint32_t count;
    DrawableReqMask mask;
  };

  struct State {
    CG_NS::GrState::Ptr state;
    uint32_t count;
    DrawableReqMask mask;
    uint32_t vertShaderIndex;
    uint32_t fragShaderIndex;
    uint32_t tableIndex;
  };

  std::vector<Node*> drawableNodes_{};
  std::vector<Drawable> blendDrawables_{};
  std::vector<Drawable> opaqueDrawables_{};
  std::vector<Shader> vertShaders_{};
  std::vector<Shader> fragShaders_{};
  std::vector<Table> tables_{};
  std::vector<State> states_{};

  void pushDrawables(Node& node, Mesh& mesh, Skin* skin);
  void processGraph(Scene&);
};

SG_NS_END

#endif // YF_SG_NEWRENDERER_H
