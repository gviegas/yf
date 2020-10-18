//
// cg
// ShaderTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "yf/cg/Shader.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ShaderTest : Test {
  ShaderTest() : Test(L"Shader") {}

  Assertions run(const vector<string>&) {
    struct Shader_ : Shader {
      Shader_(Stage stage, wstring&& codeFile, wstring&& entryPoint)
        : Shader(stage, move(codeFile), move(entryPoint)) {}
    };

    Assertions a;

    const wstring code = L"path/to/code";
    const wstring entry = L"_main0";
    Shader shd(StageFragment, wstring(code), wstring(entry));

    a.push_back({L"Shader(StageFragment, "+code+L", "+entry+L")",
                 shd.stage == StageFragment && shd.codeFile == code &&
                 shd.entryPoint == entry});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::shaderTest() {
  static ShaderTest test;
  return &test;
}
