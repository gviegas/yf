//
// yf
// DcTableTest.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "CGDcTable.h"
#include "Defs.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct DcTableTest : Test {
  DcTableTest() : Test(L"CGDcTable") {}

  Assertions run(const vector<string>& args) {
    struct DcTable : CGDcTable {
      DcTable(const CGDcEntries& entries) : CGDcTable(entries) {}
      DcTable(CGDcEntries&& entries) : CGDcTable(entries) {}

      CGResult alloc(uint32_t) { return CGResult::Success; }
      uint32_t allocations() { return UINT32_MAX; }
      CGResult write(uint32_t, CGDcId, uint32_t, CGBuffer&, uint64_t, uint64_t)
      { return CGResult::Success; }
      CGResult write(uint32_t, CGDcId, uint32_t, CGImage&, uint32_t)
      { return CGResult::Success; }
    };

    Assertions a;

    const CGDcEntries ents{{4, {CGDcTypeStorage,    1}},
                           {2, {CGDcTypeUniform,    1}},
                           {0, {CGDcTypeImgSampler, 8}}};
    DcTable dt1(ents);
    DcTable dt2({{6, {CGDcTypeImage, 32}}});

    a.push_back({L"CGDcTable(#const CGDcEntries#)",
                 dt1.entries.size() == 3 &&
                 dt1.entries.find(0)->second.type == CGDcTypeImgSampler &&
                 dt1.entries.find(0)->second.elements == 8 &&
                 dt1.entries.find(2)->second.type == CGDcTypeUniform &&
                 dt1.entries.find(2)->second.elements == 1 &&
                 dt1.entries.find(4)->second.type == CGDcTypeStorage &&
                 dt1.entries.find(4)->second.elements == 1});

    a.push_back({L"CGDcTable({{6, {CGDcTypeImage, 32}})",
                 dt2.entries.size() == 1 &&
                 dt2.entries.find(6)->second.type == CGDcTypeImage &&
                 dt2.entries.find(6)->second.elements == 32});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::dcTableTest() {
  static DcTableTest test;
  return &test;
}
