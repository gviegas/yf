//
// CG
// Shader.cxx
//
// Copyright © 2023 Gustavo C. Viegas.
//

#include <stdexcept>

#include "Shader.h"

using namespace CG_NS;
using namespace std;

Shader::Shader(const Desc& desc)
  : stage_(desc.stage), entryPoint_(desc.entryPoint) {

  switch (stage_) {
  case StageVertex:
  case StageFragment:
  case StageCompute:
    break;
  default:
    throw invalid_argument("Shader::Desc::stage is invalid");
  }

  if (entryPoint_.empty())
    throw invalid_argument("Shader::Desc::entryPoint is invalid");

  if (desc.codeFile.empty())
    throw invalid_argument("Shader::Desc::codeFile is invalid");
}

Shader::~Shader() { }

Stage Shader::stage() const {
  return stage_;
}

const string& Shader::entryPoint() const {
  return entryPoint_;
}
