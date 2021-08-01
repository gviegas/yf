//
// SG
// Renderer.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_RENDERER_H
#define YF_SG_RENDERER_H

#include <vector>
#include <unordered_map>

#include "yf/cg/Pass.h"
#include "yf/cg/DcTable.h"
#include "yf/cg/State.h"
#include "yf/cg/Queue.h"
#include "yf/cg/Buffer.h"

#include "Scene.h"
#include "Model.h"
#include "Mesh.h"
#include "Skin.h"
#include "Material.h"

SG_NS_BEGIN

/// Renderer.
///
class Renderer {
 public:
  Renderer();
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  ~Renderer() = default;

  /// Renders a scene on a given target.
  ///
  void render(Scene& scene, CG_NS::Target& target);

 private:
  Scene* prevScene_{};
  CG_NS::Pass* prevPass_{};
  CG_NS::DcTable::Ptr glbTable_{};
  CG_NS::CmdBuffer::Ptr cmdBuffer_{};
  CG_NS::Buffer::Ptr unifBuffer_{};

  /// Key for the model map.
  ///
  struct MdlKey {
    Mesh mesh{};
    Material material{};

    bool operator==(const MdlKey& other) const {
      return mesh == other.mesh && material == other.material;
    }
  };

  /// Hasher for the model map.
  ///
  struct MdlHash {
    size_t operator()(const MdlKey& k) const {
      return k.mesh.hash() ^ k.material.hash();
    }
  };

  /// Value for the model map.
  ///
  using MdlValue = std::vector<Model*>;

  std::unordered_map<MdlKey, MdlValue, MdlHash> models_{};

  /// Resource for rendering.
  ///
  struct Resource {
    std::vector<CG_NS::Shader::Ptr> shaders{};
    CG_NS::DcTable::Ptr table{};
    CG_NS::GrState::Ptr state{};

    void reset() {
      state.reset();
      table.reset();
      shaders.clear();
    }
  };

  // TODO: resources for different kinds of models (e.g., points primitives)
  Resource resource_{};
  Resource resource2_{};
  Resource resource4_{};
  Resource resource8_{};
  Resource resource16_{};
  Resource resource32_{};

  /// Processes a scene graph.
  ///
  void processGraph(Scene& scene);

  /// Prepares for rendering.
  ///
  void prepare();

#ifdef YF_DEVEL
  void print() const;
#endif
};

SG_NS_END

#endif // YF_SG_RENDERER_H
