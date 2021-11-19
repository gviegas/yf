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
#include <utility>

#include "yf/cg/Queue.h"
#include "yf/cg/Pass.h"
#include "yf/cg/Buffer.h"
#include "yf/cg/Shader.h"
#include "yf/cg/DcTable.h"
#include "yf/cg/State.h"
#include "yf/cg/Encoder.h"

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

  Scene* scene_{};
  CG_NS::Pass* pass_{};

  CG_NS::Viewport viewport_{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
  CG_NS::Scissor scissor_{{0}, {0}};

  enum DrawableReq {
    // Which material (default is PBRMR)
    RPbrsg = 1 << 0,
    RUnlit = 1 << 1,

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
    uint64_t unifSize;
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

  void processGraph();
  void pushDrawables(Node&, Mesh&, Skin*);
  template<class T>
    std::pair<uint32_t, bool> getIndex(DrawableReqMask, const std::vector<T>&);
  bool setState(Drawable&);
  bool setShaders(DrawableReqMask, CG_NS::GrState::Config&,
                  uint32_t& vertShaderIndex, uint32_t& fragShaderIndex);
  bool setTables(DrawableReqMask, CG_NS::GrState::Config&,
                 uint32_t& tableIndex);
  void setInputs(DrawableReqMask, CG_NS::GrState::Config&);

  std::vector<uint32_t> tableAllocations_{};

  void allocateTables();
  void allocateTablesSubset();

  static constexpr uint32_t ViewportN = 1;

  struct Viewport {
    float x;
    float y;
    float width;
    float height;
    float zNear;
    float zFar;

    float pad1, pad2;
  };

  static_assert(sizeof(Viewport) == 32);

  struct Global {
    float v[16];
    float p[16];
    float vp[16];
    float o[16];
    Viewport vport[ViewportN];
  };

  static_assert(sizeof(Global) == 256 + sizeof(Viewport) * ViewportN);

  static constexpr uint32_t LightN = 16;

  struct LightSource {
    int32_t notUsed;
    int32_t lightType;
    float intensity;
    float range;
    float color[3];
    float angularScale;
    float position[3];
    float angularOffset;
    float direction[3];

    float pad1;
  };

  static_assert(sizeof(LightSource) == 64);

  struct Light {
    LightSource l[LightN];
  };

  static_assert(sizeof(Light) == sizeof(LightSource) * LightN);

  static constexpr uint32_t InstanceN = 1;
  static constexpr uint32_t JointN = 100;

  struct PerInstanceWithSkin {
    float m[16];
    float mv[16];
    float norm[16];
    float joints[16 * JointN];
    float normJoints[16 * JointN];
  };

  static_assert(sizeof(PerInstanceWithSkin) == 192 + 128 * JointN);

  struct PerInstanceNoSkin {
    float m[16];
    float mv[16];
    float norm[16];
  };

  static_assert(sizeof(PerInstanceNoSkin) == 192);

  struct InstanceWithSkin {
    PerInstanceWithSkin i[InstanceN];
  };

  static_assert(sizeof(InstanceWithSkin) ==
                sizeof(PerInstanceWithSkin) * InstanceN);

  struct InstanceNoSkin {
    PerInstanceNoSkin i[InstanceN];
  };

  static_assert(sizeof(InstanceNoSkin) ==
                sizeof(PerInstanceNoSkin) * InstanceN);

  struct MaterialPbr {
    float colorFac[4];
    float alphaCutoff;
    int32_t doubleSided;
    float normalFac;
    float occlusionFac;
    float pbrFac[4];
    float emissiveFac[3];

    float pad1;
  };

  static_assert(sizeof(MaterialPbr) == 64);

  struct MaterialUnlit {
    float colorFac[4];
    float alphaCutoff;
    int32_t doubleSided;

    float pad1, pad2;
  };

  static_assert(sizeof(MaterialUnlit) == 32);

  uint64_t globalPad_{};
  uint64_t lightPad_{};
  uint64_t instanceWithSkinPad_{};
  uint64_t instanceNoSkinPad_{};
  uint64_t materialPbrPad_{};
  uint64_t materialUnlitPad_{};

  void writeGlobal(uint64_t& offset);
  void writeLight(uint64_t& offset);
  void writeInstanceWithSkin(uint64_t& offset, Drawable&, uint32_t allocation);
  void copyInstanceSkin(PerInstanceWithSkin&, Drawable&);
  void writeInstanceNoSkin(uint64_t& offset, Drawable&, uint32_t allocation);
  void writeMaterialPbr(uint64_t& offset, Drawable&, uint32_t allocation);
  void writeMaterialUnlit(uint64_t& offset, Drawable&, uint32_t allocation);
  void writeTextureMaps(Drawable&, uint32_t allocation);
};

SG_NS_END

#endif // YF_SG_NEWRENDERER_H
