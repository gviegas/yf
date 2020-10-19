//
// cg
// ResultTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "yf/cg/Result.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ResultTest : Test {
  ResultTest() : Test(L"Result") {}

  Assertions run(const vector<string>&) {
    Assertions a;

    Result ok1(Result::Success);
    Result ok2(Result::Success);
    Result nok(Result::Failure);

    a.push_back({L"Result ok1(Result::Success)",
                 ok1.value == Result::Success});
    a.push_back({L"Result ok2(Result::Success)",
                 ok2.value == Result::Success});
    a.push_back({L"Result nok(Result::Failure)",
                 nok.value == Result::Failure});
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
