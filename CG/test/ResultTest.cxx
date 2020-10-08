//
// yf
// ResultTest.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include <iostream>

#include "UnitTests.h"
#include "CGResult.h"
#include "Defs.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ResultTest : Test {
  ResultTest() : Test(L"CGResult") {}

  Assertions run(const vector<string>& args) {
    Assertions a;

    CGResult ok1(CGResult::Success);
    CGResult ok2(CGResult::Success);
    CGResult nok(CGResult::Failure);

    a.push_back({L"CGResult ok1(CGResult::Success)",
                 ok1.value == CGResult::Success});
    a.push_back({L"CGResult ok2(CGResult::Success)",
                 ok2.value == CGResult::Success});
    a.push_back({L"CGResult nok(CGResult::Failure)",
                 nok.value == CGResult::Failure});
    a.push_back({L"ok1", ok1});
    a.push_back({L"!ok2", !(!ok2)});
    a.push_back({L"ok1 == ok2", ok1 == ok2});
    a.push_back({L"ok2 != nok", ok2 != nok});
    a.push_back({L"!nok", !nok});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::resultTest() {
  static ResultTest test;
  return &test;
}
