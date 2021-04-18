//
// SG
// Renderer.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_RENDERER_H
#define YF_SG_RENDERER_H

#include <unordered_map>

#include "yf/cg/Pass.h"

#include "Scene.h"
#include "Model.h"

SG_NS_BEGIN

/// Renderer.
///
class Renderer {
 public:
  Renderer() = default;
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  ~Renderer() = default;

  /// Renders a scene on a given target.
  ///
  void render(Scene& scene, CG_NS::Target& target);

 private:
  // TODO...
  Scene* prevScene_{};

  /// Key for the model map.
  ///
  struct MdlKey {
    Mesh* mesh{};
    Material* material{};

    bool operator==(const MdlKey& other) const {
      return mesh == other.mesh && material == other.material;
    }
  };

  /// Hasher for the model map.
  ///
  struct MdlHash {
    size_t operator()(const MdlKey& k) const {
      return std::hash<void*>()(k.mesh) ^ std::hash<void*>()(k.material);
    }
  };

  /// Value for the model map.
  ///
  using MdlValue = std::vector<Model*>;

  std::unordered_map<MdlKey, MdlValue, MdlHash> models_{};

  /// Processes a scene graph.
  ///
  void processGraph(Scene& scene);
};

SG_NS_END

#endif // YF_SG_RENDERER_H
