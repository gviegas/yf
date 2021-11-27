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

  Shader() = default;
  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;
  virtual ~Shader() = default;

  /// Getters.
  ///
  virtual Stage stage() const = 0;
  virtual const std::string& entryPoint() const = 0;
};

CG_NS_END

#endif // YF_CG_SHADER_H
