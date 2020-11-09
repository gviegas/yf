//
// cg
// DcTableTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "yf/cg/DcTable.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct DcTableTest : Test {
  DcTableTest() : Test(L"DcTable") {}

  Assertions run(const vector<string>&) {
    struct DcTable_ : DcTable {
      DcTable_(const DcEntries& entries) : DcTable(entries) {}
      DcTable_(DcEntries&& entries) : DcTable(entries) {}

      void allocate(uint32_t) {
      }
      uint32_t allocations() {
        return UINT32_MAX;
      }
      void write(uint32_t, DcId, uint32_t, Buffer&, uint64_t, uint64_t) {
      }
      void write(uint32_t, DcId, uint32_t, Image&, uint32_t) {
      }
    };

    Assertions a;

    const DcEntries ents{{4, {DcTypeStorage,    1}},
                         {2, {DcTypeUniform,    1}},
                         {0, {DcTypeImgSampler, 8}}};
    DcTable_ dt1(ents);
    DcTable_ dt2({{6, {DcTypeImage, 32}}});

    a.push_back({L"DcTable(#const DcEntries#)",
                 dt1.entries.size() == 3 &&
                 dt1.entries.find(0)->second.type == DcTypeImgSampler &&
                 dt1.entries.find(0)->second.elements == 8 &&
                 dt1.entries.find(2)->second.type == DcTypeUniform &&
                 dt1.entries.find(2)->second.elements == 1 &&
                 dt1.entries.find(4)->second.type == DcTypeStorage &&
                 dt1.entries.find(4)->second.elements == 1});

    a.push_back({L"DcTable({{6, {DcTypeImage, 32}})",
                 dt2.entries.size() == 1 &&
                 dt2.entries.find(6)->second.type == DcTypeImage &&
                 dt2.entries.find(6)->second.elements == 32});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::dcTableTest() {
  static DcTableTest test;
  return &test;
}
