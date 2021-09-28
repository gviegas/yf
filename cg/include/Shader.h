//
// CG
// Shader.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_SHADER_H
#define YF_CG_SHADER_H

#include <memory>
#include <string>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

/// Programmable shader stages.
///
enum Stage {
  StageVertex,
  StageTesCtrl,
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

  Shader(Stage stage, const std::string& codeFile,
         const std::string& entryPoint = "main");

  virtual ~Shader() = 0;

  /// Instance constants.
  ///
  const Stage stage_;
  const std::string codeFile_;
  const std::string entryPoint_;
};

CG_NS_END

#endif // YF_CG_SHADER_H
