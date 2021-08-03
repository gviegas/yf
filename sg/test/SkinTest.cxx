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

    Skin skin1(1, {m});
    Skin skin2(1, {});
    Skin skin3(3, {m, m, m});
    Skin skin4(3, {});

    try {
      Skin skin(1, {m, m});
      check = false;
    } catch (...) { }

    try {
      Skin skin(2, {m});
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
