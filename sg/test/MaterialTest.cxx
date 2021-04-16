//
// SG
// MaterialTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "UnitTests.h"
#include "Material.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct MaterialTest : Test {
  MaterialTest() : Test(L"Material") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    // TODO
    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::materialTest() {
  static MaterialTest test;
  return &test;
}
