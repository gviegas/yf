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
      Shader_(Stage stage, string&& codeFile, string&& entryPoint)
        : Shader(stage, codeFile, entryPoint) { }
    };

    Assertions a;

    const string code = "path/to/code";
    const string entry = "_main0";
    Shader_ shd(StageFragment, string(code), string(entry));

    a.push_back({L"Shader(StageFragment, \"path/to/code\", \"_main0\")",
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
