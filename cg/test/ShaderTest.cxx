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
    class Shader_ : public Shader {
      Stage stage_;
      string codeFile_;
      string entryPoint_;
     public:
      Shader_(Stage stage, string&& codeFile, string&& entryPoint)
        : stage_(stage), codeFile_(codeFile), entryPoint_(entryPoint) { }
      Stage stage() const { return stage_; }
      const std::string& entryPoint() const { return entryPoint_; }
    };

    Assertions a;

    const string code = "path/to/code";
    const string entry = "_main0";
    Shader_ shd(StageFragment, string(code), string(entry));

    a.push_back({L"Shader(StageFragment, \"path/to/code\", \"_main0\")",
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
