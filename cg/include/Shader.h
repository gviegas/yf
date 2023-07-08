//
// CG
// Shader.h
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#ifndef YF_CG_SHADER_H
#define YF_CG_SHADER_H

#include <cstdint>
#include <memory>
#include <string>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

/// Mask of `Stage` bits.
///
using StageMask = uint32_t;

/// Programmable shader stages.
///
enum Stage : uint32_t {
  StageVertex   = 0x01,
  StageFragment = 0x02,
  StageCompute  = 0x04
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
