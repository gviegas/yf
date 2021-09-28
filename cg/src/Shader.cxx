//
// CG
// Shader.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Shader.h"

using namespace CG_NS;
using namespace std;

Shader::Shader(Stage stage, const string& codeFile, const string& entryPoint)
  : stage_(stage), codeFile_(codeFile), entryPoint_(entryPoint) { }

Shader::~Shader() { }
