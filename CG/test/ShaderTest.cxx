//
// yf
// ShaderTest.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include <iostream>

#include "Defs.h"
#include "UnitTests.h"
#include "CGShader.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ShaderTest : Test {
  ShaderTest() : Test(L"CGShader") {}

  Assertions run(const vector<string>& args) {
    struct Shader : CGShader {
      Shader(CGStage stage, wstring&& codeFile, wstring&& entryPoint)
        : CGShader(stage, move(codeFile), move(entryPoint)) {}
    };

    Assertions a;

    const wstring code = L"path/to/code";
    const wstring entry = L"_main0";
    Shader shd(CGStageFragment, wstring(code), wstring(entry));

    a.push_back({L"CGShader(CGStageFragment, "+code+L", "+entry+L")",
                 shd.stage == CGStageFragment && shd.codeFile == code &&
                 shd.entryPoint == entry});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::shaderTest() {
  static ShaderTest test;
  return &test;
}
