//
// CG
// TypesTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "Types.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct TypesTest : Test {
  TypesTest() : Test(L"Types") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    // Size2
    {
      Size2 t(20);
      Size2 u(1, 2);
      a.push_back({L"Size2 t(20)", t.width == 20 && t.height == 20});
      a.push_back({L"Size2 u(1, 2)", u.width == 1 && u.height == 2});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == Size2(20, 20)", t == Size2(20, 20)});
      a.push_back({L"u != Size2(2, 1)", u != Size2(2, 1)});
    }

    // Size3
    {
      Size3 t(30);
      Size3 u(1, 2, 3);
      a.push_back({L"Size3 t(30)",
                   t.width == 30 && t.height == 30 && t.depth == 30});
      a.push_back({L"Size3 u(1, 2, 3)",
                   u.width == 1 && u.height == 2 && u.depth == 3});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == Size3(30, 30, 30)", t == Size3(30, 30, 30)});
      a.push_back({L"u != Size3(1, 2, 4)", u != Size3(1, 2, 4)});
      a.push_back({L"u != Size3({1, 2}, 3)", !(u != Size3({1, 2}, 3))});
    }

    // Offset2
    {
      Offset2 t(-20);
      Offset2 u(-1, 2);
      a.push_back({L"Offset2 t(-20)", t.x == -20 && t.y == -20});
      a.push_back({L"Offset2 u(1, 2)", u.x == -1 && u.y == 2});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == Offset2(-20, -20)", t == Offset2(-20, -20)});
      a.push_back({L"u != Offset2(2, -1)", u != Offset2(2, -1)});
    }

    // Offset3
    {
      Offset3 t(3);
      Offset3 u(-1, 2, -3);
      a.push_back({L"Offset3 t(3)", t.x == 3 && t.y == 3 && t.z == 3});
      a.push_back({L"Offset3 u(1, 2)", u.x == -1 && u.y == 2 && u.z == -3});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == Offset3(3, 3, 3)", t == Offset3(3, 3, 3)});
      a.push_back({L"u != Offset3(3, -1, 1)", u != Offset3(3, -1, 1)});
      a.push_back({L"u == Offset3({-1, 2}, -3)", u == Offset3({-1, 2}, -3)});
    }

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::typesTest() {
  static TypesTest test;
  return &test;
}
