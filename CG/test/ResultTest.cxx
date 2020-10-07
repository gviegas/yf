//
// yf
// ResultTest.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include <iostream>

#include "Defs.h"
#include "UnitTests.h"
#include "CGResult.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ResultTest : Test {
  ResultTest() : Test(L"CGResult") {}

  Coverage run(const vector<string>& args) {
    CGResult ok1(CGResult::Success);
    CGResult ok2(CGResult::Success);
    CGResult nok(CGResult::Failure);

    wcout << "\n-Result-"
          << "\nok1 == ok2 : " << (ok1 == ok2)
          << "\nok1 : " << ok1
          << "\n!ok1 : " << !ok1
          << "\nok1 == nok : " << (ok1 == nok)
          << "\nok1 != nok : " << (ok1 != nok)
          << "\nnok : " << nok
          << "\n!nok : " << !nok
          << endl;

    // TODO
    return 0.95;
  }
};

INTERNAL_NS_END

Test* TEST_NS::resultTest() {
  static ResultTest test;
  return &test;
}
