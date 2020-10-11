//
// yf
// CGShader.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "CGShader.h"

using namespace YF_NS;
using namespace std;

CGShader::CGShader(CGStage stage, wstring&& codeFile, wstring&& entryPoint)
  : stage(stage), codeFile(codeFile), entryPoint(entryPoint) {}

CGShader::~CGShader() {}
