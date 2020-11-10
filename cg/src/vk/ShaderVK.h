//
// CG
// ShaderVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_SHADERVK_H
#define YF_CG_SHADERVK_H

#include "yf/cg/Shader.h"
#include "VK.h"

CG_NS_BEGIN

class ShaderVK final : public Shader {
 public:
  ShaderVK(Stage stage, std::wstring&& codeFile, std::wstring&& entryPoint);
  ~ShaderVK();

  VkShaderModule module() const;

 private:
  VkShaderModule module_ = VK_NULL_HANDLE;
};

CG_NS_END

#endif // YF_CG_SHADERVK_H
