//
// SG
// SceneTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "Scene.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct SceneTest : Test {
  SceneTest() : Test(L"Scene") { }

  Assertions run(const vector<string>&) {
    // TODO
    Scene scn;
    return {{L"Scene()", scn.isRoot() && scn.isLeaf()}};
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* sceneTest() {
  static SceneTest test;
  return &test;
}

TEST_NS_END
