//
// yf
// TypesTest.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include <iostream>

#include "Defs.h"
#include "UnitTests.h"
#include "CGTypes.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct TypesTest : Test {
  TypesTest() : Test(L"CGTypes") {}

  Coverage run(const vector<string>& args) {
    vector<pair<wstring, bool>> res;

    // CGSize2
    {
      CGSize2 a(20);
      CGSize2 b(1, 2);
      res.push_back({L"CGSize2 a(20)", a.width == 20 && a.height == 20});
      res.push_back({L"CGSize2 b(1, 2)", b.width == 1 && b.height == 2});
      res.push_back({L"a == b", !(a == b)});
      res.push_back({L"a == CGSize2(20, 20)", a == CGSize2(20, 20)});
      res.push_back({L"b != CGSize2(2, 1)", b != CGSize2(2, 1)});
    }

    // CGSize3
    {
      CGSize3 a(30);
      CGSize3 b(1, 2, 3);
      res.push_back({L"CGSize3 a(30)",
                     a.width == 30 && a.height == 30 && a.depth == 30});
      res.push_back({L"CGSize3 b(1, 2, 3)",
                     b.width == 1 && b.height == 2 && b.depth == 3});
      res.push_back({L"a == b", !(a == b)});
      res.push_back({L"a == CGSize3(30, 30, 30)", a == CGSize3(30, 30, 30)});
      res.push_back({L"b != CGSize3(1, 2, 4)", b != CGSize3(1, 2, 4)});
      res.push_back({L"b != CGSize3({1, 2}, 3)", !(b != CGSize3({1, 2}, 3))});
    }

    // CGOffset2
    {
      CGOffset2 a(-20);
      CGOffset2 b(-1, 2);
      res.push_back({L"CGOffset2 a(-20)", a.x == -20 && a.y == -20});
      res.push_back({L"CGOffset2 b(1, 2)", b.x == -1 && b.y == 2});
      res.push_back({L"a == b", !(a == b)});
      res.push_back({L"a == CGOffset2(-20, -20)", a == CGOffset2(-20, -20)});
      res.push_back({L"b != CGOffset2(2, -1)", b != CGOffset2(2, -1)});
    }

    // CGOffset3
    {
      CGOffset3 a(3);
      CGOffset3 b(-1, 2, -3);
      res.push_back({L"CGOffset3 a(3)", a.x == 3 && a.y == 3 && a.z == 3});
      res.push_back({L"CGOffset3 b(1, 2)", b.x == -1 && b.y == 2 && b.z == -3});
      res.push_back({L"a == b", !(a == b)});
      res.push_back({L"a == CGOffset3(3, 3, 3)", a == CGOffset3(3, 3, 3)});
      res.push_back({L"b != CGOffset3(3, -1, 1)", b != CGOffset3(3, -1, 1)});
      res.push_back({L"b == CGOffset3({-1, 2}, -3)",
                     b == CGOffset3({-1, 2}, -3)});
    }

    double cov = 0.0;
    for (auto& p : res) {
      wcout << "\n@ " << p.first << (p.second ? "\nPASSED\n" : "\nFAILED\n");
      cov += p.second;
    }

    return cov / res.size();
  }
};

INTERNAL_NS_END

Test* TEST_NS::typesTest() {
  static TypesTest test;
  return &test;
}
