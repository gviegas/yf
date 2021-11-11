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
class Primitive;

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
    // Has given vertex attributes
    RNormal    = 0x01,
    RTangent   = 0x02,
    RTexCoord0 = 0x04,
    RTexCoord1 = 0x08,
    RColor0    = 0x10,

    // Has `Skin`, `Joints0` and `Weights0`
    RSkin0 = 0x20,

    // reserved = 0x40
    // reserved = 0x80

    // Has `Material`
    RMaterial = 0x100,

    // reserved = 0x200
    // reserved = 0x400

    // Has given texture maps
    RColorMap     =  0x800,
    RPbrMap       = 0x1000,
    RNormalMap    = 0x2000,
    ROcclusionMap = 0x4000,
    REmissiveMap  = 0x8000
  };

  using DrawableReqMask = uint32_t;

  struct Drawable {
    size_t nodeIndex;
    Primitive& primitive;
    DrawableReqMask mask;
  };

  std::vector<Node*> nodes_{};
  std::vector<Drawable> blendDrawables_{};
  std::vector<Drawable> opaqueDrawables_{};

  void processGraph(Node&);
};

SG_NS_END

#endif // YF_SG_NEWRENDERER_H
