//
// CG
// Shader.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Shader.h"

using namespace CG_NS;
using namespace std;

Shader::Shader(Stage stage, const wstring& codeFile, const wstring& entryPoint)
  : stage_(stage), codeFile_(codeFile), entryPoint_(entryPoint) { }

Shader::~Shader() { }
