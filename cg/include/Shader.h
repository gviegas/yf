//
// CG
// Shader.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_SHADER_H
#define YF_CG_SHADER_H

#include <string>
#include <memory>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

/// Programmable shader stages.
///
enum Stage {
  StageVertex,
  StageTesCntrl,
  StageTesEval,
  StageGeometry,
  StageFragment,
  StageCompute
};

/// Shader stage module.
///
class Shader {
 public:
  using Ptr = std::unique_ptr<Shader>;

  Shader(Stage stage,
         std::wstring&& codeFile,
         std::wstring&& entryPoint = L"main");

  virtual ~Shader() = 0;

  /// Instance constants.
  ///
  const Stage        stage;
  const std::wstring codeFile;
  const std::wstring entryPoint;
};

CG_NS_END

#endif // YF_CG_SHADER_H
