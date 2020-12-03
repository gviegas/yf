//
// SG
// SceneTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
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

Test* TEST_NS::sceneTest() {
  static SceneTest test;
  return &test;
}
