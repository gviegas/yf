//
// CG
// Shader.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/cg/Shader.h"

using namespace CG_NS;
using namespace std;

Shader::Shader(Stage stage, wstring&& codeFile, wstring&& entryPoint)
  : stage_(stage), codeFile_(codeFile), entryPoint_(entryPoint) { }

Shader::~Shader() {}
