//
// CG
// CopyTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "CG.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct CopyTest : Test {
  CopyTest() : Test(L"Copy") { }

  Assertions run(const vector<string>&) {
    // TODO
    return {};
  }
};

INTERNAL_NS_END

Test* TEST_NS::copyTest() {
  static CopyTest test;
  return &test;
}
