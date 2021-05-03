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

    // TODO...
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
