//
// SG
// SkinTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "Skin.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct SkinTest : Test {
  SkinTest() : Test(L"Skin") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    const auto m = Mat4f::identity();
    bool check = true;

    Skin skin1({m}, {m});
    Skin skin2({m}, {});
    Skin skin3({m, m, m}, {m, m, m});
    Skin skin4({m, m, m}, {});

    try {
      Skin skin({}, {});
      check = false;
    } catch (...) { }

    try {
      Skin skin({m, m}, {m});
      check = false;
    } catch (...) { }

    a.push_back({L"Skin(...)", skin1.joints().size() == 1 &&
                               skin1.inverseBind().size() == 1 &&
                               skin2.joints().size() == 1 &&
                               skin2.inverseBind().empty() &&
                               skin3.joints().size() == 3 &&
                               skin3.inverseBind().size() == 3 &&
                               skin4.joints().size() == 3 &&
                               skin4.inverseBind().empty() && check});
    Skin skin5;

    a.push_back({L"Skin()", !skin5});

    Skin skin6(skin4);

    a.push_back({L"Skin(skin)",
                 skin6.joints().size() == skin4.joints().size() &&
                 skin6.inverseBind().size() == skin4.inverseBind().size()});

    Skin skin7 = skin3;

    a.push_back({L"=",
                 skin7.joints().size() == skin3.joints().size() &&
                 skin7.inverseBind().size() == skin3.inverseBind().size()});

    Skin skin8;

    a.push_back({L"bool, !", skin1 && skin2 && skin3 && skin4 && !skin5 &&
                             skin6 && skin7 && !skin8});

    a.push_back({L"==, !=", skin1 != skin5 && skin5 != skin2 &&
                            skin2 != skin1 && skin3 == skin7 &&
                            skin6 == skin4 && skin5 == skin8});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* skinTest() {
  static SkinTest test;
  return &test;
}

TEST_NS_END
