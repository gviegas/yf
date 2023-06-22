//
// CG
// TypesTest.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#include "Test.h"
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
      Size2 t(20, 20);
      Size2 u(1, 2);
      a.push_back({L"Size2 t(20, 20)", t.width == 20 && t.height == 20});
      a.push_back({L"Size2 u(1, 2)", u.width == 1 && u.height == 2});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == Size2(20, 20)", t == Size2(20, 20)});
      a.push_back({L"u != Size2(2, 1)", u != Size2(2, 1)});
    }

    // Size3
    {
      Size3 t(30, 30, 30);
      Size3 u(1, 2, 3);
      a.push_back({L"Size3 t(30, 30, 30)",
                   t.width == 30 && t.height == 30 && t.depthOrLayers == 30});
      a.push_back({L"Size3 u(1, 2, 3)",
                   u.width == 1 && u.height == 2 && u.depthOrLayers == 3});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == Size3(30, 30, 30)", t == Size3(30, 30, 30)});
      a.push_back({L"u != Size3(1, 2, 4)", u != Size3(1, 2, 4)});
      a.push_back({L"u != Size3({1, 2}, 3)", !(u != Size3({1, 2}, 3))});
    }

    // Offset2
    {
      Offset2 t(-20, -20);
      Offset2 u(-1, 2);
      Offset2 v;
      a.push_back({L"Offset2 t(-20, -20)", t.x == -20 && t.y == -20});
      a.push_back({L"Offset2 u(1, 2)", u.x == -1 && u.y == 2});
      a.push_back({L"Offset2 v", v.x == 0 && v.y == 0});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == Offset2(-20, -20)", t == Offset2(-20, -20)});
      a.push_back({L"u != Offset2(2, -1)", u != Offset2(2, -1)});
      a.push_back({L"v == Offset2(0, 0)", v == Offset2(0, 0)});
    }

    // Offset3
    {
      Offset3 t(3, 3, 3);
      Offset3 u(-1, 2, -3);
      Offset3 v;
      a.push_back({L"Offset3 t(3, 3, 3)", t.x == 3 && t.y == 3 && t.z == 3});
      a.push_back({L"Offset3 u(1, 2)", u.x == -1 && u.y == 2 && u.z == -3});
      a.push_back({L"Offset3 v", v.x == 0 && v.y == 0 && v.z == 0});
      a.push_back({L"t == u", !(t == u)});
      a.push_back({L"t == Offset3(3, 3, 3)", t == Offset3(3, 3, 3)});
      a.push_back({L"u != Offset3(3, -1, 1)", u != Offset3(3, -1, 1)});
      a.push_back({L"u == Offset3({-1, 2}, -3)", u == Offset3({-1, 2}, -3)});
      a.push_back({L"v == Offset3(0, 0, 0)", v == Offset3(0, 0, 0)});
      a.push_back({L"v == Offset3(Offset2{}, 0)", v == Offset3(Offset2{}, 0)});
    }

    // Range
    {
      Range t(0, 10);
      Range u(3, 4);
      Range v(0, 0);
      Range w(3, 3);
      a.push_back({L"Range (0, 10)", t.start == 0 && t.end == 10});
      a.push_back({L"Range (3, 4)", u.start == 3 && u.end == 4});
      a.push_back({L"Range (0, 0)", v.start == 0 && v.end == 0});
      a.push_back({L"Range (3, 3)", w.start == 3 && w.end == 3});
      a.push_back({L"t.count()", t.count() == 10});
      a.push_back({L"u.count()", u.count() == 1});
      a.push_back({L"v.count()", v.count() == 0});
      a.push_back({L"w.count()", w.count() == 0});
      a.push_back({L"t.contains(t)", t.contains(t)});
      a.push_back({L"t.contains(u)", t.contains(u)});
      a.push_back({L"t.contains(v)", t.contains(v)});
      a.push_back({L"t.contains(w)", t.contains(w)});
      a.push_back({L"u.contains(t)", !u.contains(t)});
      a.push_back({L"u.contains(u)", u.contains(u)});
      a.push_back({L"u.contains(v)", !u.contains(v)});
      a.push_back({L"u.contains(w)", u.contains(w)});
      a.push_back({L"v.contains(t)", !v.contains(t)});
      a.push_back({L"v.contains(u)", !v.contains(u)});
      a.push_back({L"v.contains(v)", v.contains(v)});
      a.push_back({L"v.contains(w)", !v.contains(w)});
      a.push_back({L"w.contains(t)", !w.contains(t)});
      a.push_back({L"w.contains(u)", !w.contains(u)});
      a.push_back({L"w.contains(v)", !w.contains(v)});
      a.push_back({L"w.contains(w)", w.contains(w)});
    }

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* typesTest() {
  static TypesTest test;
  return &test;
}

TEST_NS_END
