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
    size_t index = 0;
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

    for (const auto& sk : {&skin1, &skin2, &skin3, &skin4}) {
      for (const auto& jt : sk->joints()) {
        if (&jt.skin_ != sk || jt.index_ != index++)
          check = false;
      }
      index = 0;
    }

    a.push_back({L"Skin(...)", skin1.joints().size() == 1 &&
                               skin1.inverseBind().size() == 1 &&
                               skin2.joints().size() == 1 &&
                               skin2.inverseBind().empty() &&
                               skin3.joints().size() == 3 &&
                               skin3.inverseBind().size() == 3 &&
                               skin4.joints().size() == 3 &&
                               skin4.inverseBind().empty() && check});

    Skin skin5(skin4);

    for (const auto& jt : skin5.joints()) {
      if (&jt.skin_ != &skin5 || jt.index_ != index++)
        check = false;
    }
    index = 0;

    a.push_back({L"Skin(skin)",
                 skin5.joints().size() == skin4.joints().size() &&
                 skin5.inverseBind().size() == skin4.inverseBind().size() &&
                 check});

    Skin skin6 = skin3;

    for (const auto& jt : skin6.joints()) {
      if (&jt.skin_ != &skin6 || jt.index_ != index++)
        check = false;
    }
    index = 0;

    a.push_back({L"=",
                 skin6.joints().size() == skin3.joints().size() &&
                 skin6.inverseBind().size() == skin3.inverseBind().size() &&
                 check});

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
