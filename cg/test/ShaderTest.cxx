//
// CG
// ShaderTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "Shader.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ShaderTest : Test {
  ShaderTest() : Test(L"Shader") { }

  Assertions run(const vector<string>&) {
    struct Shader_ : Shader {
      Shader_(Stage stage, wstring&& codeFile, wstring&& entryPoint)
        : Shader(stage, move(codeFile), move(entryPoint)) { }
    };

    Assertions a;

    const wstring code = L"path/to/code";
    const wstring entry = L"_main0";
    Shader_ shd(StageFragment, wstring(code), wstring(entry));

    a.push_back({L"Shader(StageFragment, "+code+L", "+entry+L")",
                 shd.stage_ == StageFragment && shd.codeFile_ == code &&
                 shd.entryPoint_ == entry});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* shaderTest() {
  static ShaderTest test;
  return &test;
}

TEST_NS_END
