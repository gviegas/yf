//
// CG
// ShaderTest.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
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
    class Shader_ : public Shader {
     public:
      Shader_(const Desc& desc) : Shader(desc) { }
    };

    Assertions a;

    const string entry = "_main0";
    const string code = "path/to/code";
    Shader_ shd({StageFragment, string(entry), string(code)});

    a.push_back({L"Shader({StageFragment, \"_main0\", \"path/to/code\"})",
                 shd.stage() == StageFragment && shd.entryPoint() == entry});

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
