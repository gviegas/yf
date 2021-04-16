//
// SG
// ModelTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "Test.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ModelTest : Test {
  ModelTest() : Test(L"Model") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    // TODO
    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* modelTest() {
  static ModelTest test;
  return &test;
}

TEST_NS_END
