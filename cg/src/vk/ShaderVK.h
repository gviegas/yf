//
// CG
// ShaderVK.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_SHADERVK_H
#define YF_CG_SHADERVK_H

#include <stdexcept>

#include "Shader.h"
#include "VK.h"

CG_NS_BEGIN

class ShaderVK final : public Shader {
 public:
  ShaderVK(Stage stage, const std::wstring& codeFile,
           const std::wstring& entryPoint);
  ~ShaderVK();

  /// Getters.
  ///
  VkShaderModule module() const;
  const std::string& name() const;

 private:
  VkShaderModule module_ = VK_NULL_HANDLE;
  std::string name_{};
};

/// Converts from a `Stage` value.
///
inline VkShaderStageFlagBits toShaderStageVK(Stage stage) {
  switch (stage) {
  case StageVertex:   return VK_SHADER_STAGE_VERTEX_BIT;
  case StageTesCtrl:  return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
  case StageTesEval:  return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
  case StageGeometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
  case StageFragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
  case StageCompute:  return VK_SHADER_STAGE_COMPUTE_BIT;
  default:
    throw std::invalid_argument(__func__);
  }
}

CG_NS_END

#endif // YF_CG_SHADERVK_H
