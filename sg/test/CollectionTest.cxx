//
// SG
// CollectionTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "SG.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct CollectionTest : Test {
  CollectionTest() : Test(L"Collection") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    // TODO...

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* collectionTest() {
  static CollectionTest test;
  return &test;
}

TEST_NS_END
