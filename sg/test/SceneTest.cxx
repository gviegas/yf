//
// SG
// SceneTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <iostream>
#include <algorithm>

#include "Test.h"
#include "Scene.h"
#include "Camera.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct SceneTest : Test {
  SceneTest() : Test(L"Scene") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    Scene scn;
    a.push_back({L"Scene()", scn.isRoot() && scn.isLeaf()});

    const auto& xform = scn.camera().transform();
    auto prec = wcout.precision(3);
    wcout << "\ncamera().transform():\n";
    for (size_t i = 0; i < xform.rows(); i++) {
      wcout << "\n ";
      for (size_t j = 0; j < xform.columns(); j++)
        wcout << xform[j][i] << "\t";
    }
    wcout << endl;
    wcout.precision(prec);
    a.push_back({L"camera()", true});

    array<float, 4> color{1.0f, 1.0f, 1.0f, 1.0f};
    scn.color() = color;
    a.push_back({L"color()", equal(color.begin(), color.end(),
                                   scn.color().begin())});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* sceneTest() {
  static SceneTest test;
  return &test;
}

TEST_NS_END
