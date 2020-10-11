//
// yf
// TypesTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "CGTypes.h"
#include "Defs.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct TypesTest : Test {
  TypesTest() : Test(L"CGTypes") {}

  Assertions run(const vector<string>& args) {
    Assertions a;

    // CGSize2
    {
      CGSize2 t(20);
      CGSize2 u(1, 2);
      a.push_back({L"CGSize2 t(20)", t.width == 20 && t.height == 20});
      a.push_back({L"CGSize2 u(1, 2)", u.width == 1 && u.height == 2});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == CGSize2(20, 20)", t == CGSize2(20, 20)});
      a.push_back({L"u != CGSize2(2, 1)", u != CGSize2(2, 1)});
    }

    // CGSize3
    {
      CGSize3 t(30);
      CGSize3 u(1, 2, 3);
      a.push_back({L"CGSize3 t(30)",
                     t.width == 30 && t.height == 30 && t.depth == 30});
      a.push_back({L"CGSize3 u(1, 2, 3)",
                     u.width == 1 && u.height == 2 && u.depth == 3});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == CGSize3(30, 30, 30)", t == CGSize3(30, 30, 30)});
      a.push_back({L"u != CGSize3(1, 2, 4)", u != CGSize3(1, 2, 4)});
      a.push_back({L"u != CGSize3({1, 2}, 3)", !(u != CGSize3({1, 2}, 3))});
    }

    // CGOffset2
    {
      CGOffset2 t(-20);
      CGOffset2 u(-1, 2);
      a.push_back({L"CGOffset2 t(-20)", t.x == -20 && t.y == -20});
      a.push_back({L"CGOffset2 u(1, 2)", u.x == -1 && u.y == 2});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == CGOffset2(-20, -20)", t == CGOffset2(-20, -20)});
      a.push_back({L"u != CGOffset2(2, -1)", u != CGOffset2(2, -1)});
    }

    // CGOffset3
    {
      CGOffset3 t(3);
      CGOffset3 u(-1, 2, -3);
      a.push_back({L"CGOffset3 t(3)", t.x == 3 && t.y == 3 && t.z == 3});
      a.push_back({L"CGOffset3 u(1, 2)", u.x == -1 && u.y == 2 && u.z == -3});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == CGOffset3(3, 3, 3)", t == CGOffset3(3, 3, 3)});
      a.push_back({L"u != CGOffset3(3, -1, 1)", u != CGOffset3(3, -1, 1)});
      a.push_back({L"u == CGOffset3({-1, 2}, -3)",
                     u == CGOffset3({-1, 2}, -3)});
    }

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::typesTest() {
  static TypesTest test;
  return &test;
}
