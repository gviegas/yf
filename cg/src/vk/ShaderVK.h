//
// CG
// ShaderVK.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#ifndef YF_CG_SHADERVK_H
#define YF_CG_SHADERVK_H

#include <stdexcept>

#include "Shader.h"
#include "VK.h"

CG_NS_BEGIN

class ShaderVK final : public Shader {
 public:
  ShaderVK(const Desc& desc);
  ~ShaderVK();

  Stage stage() const;
  const std::string& entryPoint() const;

  /// Getter.
  ///
  VkShaderModule module();

 private:
  const Stage stage_{};
  const std::string entryPoint_{};
  VkShaderModule module_ = VK_NULL_HANDLE;
};

/// Converts from a `Stage` value.
///
/// This function throws if `stage` is not one of the predefined
/// shader stages. One should call `toMultipleShaderStagesVK` if
/// there may be more than one stage set.
///
inline VkShaderStageFlagBits toSingleShaderStageVK(Stage stage) {
  switch (stage) {
  case StageVertex:   return VK_SHADER_STAGE_VERTEX_BIT;
  case StageFragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
  case StageCompute:  return VK_SHADER_STAGE_COMPUTE_BIT;
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `StageMask` value.
///
/// This function throws if `mask` has no valid bits set.
///
inline VkShaderStageFlags toMultipleShaderStagesVK(StageMask mask) {
  VkShaderStageFlags flags = 0;
  if (mask & StageVertex)
    flags |= VK_SHADER_STAGE_VERTEX_BIT;
  if (mask & StageFragment)
    flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
  if (mask & StageCompute)
    flags |= VK_SHADER_STAGE_COMPUTE_BIT;
  return flags == 0 ?
         throw std::invalid_argument(__func__) :
         flags;
}

CG_NS_END

#endif // YF_CG_SHADERVK_H
