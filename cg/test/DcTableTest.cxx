//
// CG
// DcTableTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "DcTable.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct DcTableTest : Test {
  DcTableTest() : Test(L"DcTable") { }

  Assertions run(const vector<string>&) {
    class DcTable_ : public DcTable {
      vector<DcEntry> entries_;
     public:
      DcTable_(const vector<DcEntry>& entries) : entries_(entries) { }
      void allocate(uint32_t) { }
      uint32_t allocations() const { return 0; }
      void write(uint32_t, DcId, uint32_t, Buffer&, uint64_t, uint64_t) { }
      void write(uint32_t, DcId, uint32_t, Image&, uint32_t, uint32_t) { }
      void write(uint32_t, DcId, uint32_t, Image&, uint32_t, uint32_t,
                 Sampler&) { }
      const vector<DcEntry>& entries() const { return entries_; }
    };

    Assertions a;

    const vector<DcEntry> ents1{{4, DcTypeStorage, 1}, {2, DcTypeUniform, 1},
                                {0, DcTypeImgSampler, 8}};
    DcTable_ tab1(ents1);

    const vector<DcEntry> ents2{{6, DcTypeImage, 32}};
    DcTable_ tab2(ents2);

    bool chk = true;
    for (const auto& e : tab1.entries()) {
      if (e.id == 0 && (e.type != DcTypeImgSampler || e.elements != 8))
        chk = false;
      else if (e.id == 2 && (e.type != DcTypeUniform || e.elements != 1))
        chk = false;
      else if (e.id == 4 && (e.type != DcTypeStorage || e.elements != 1))
        chk = false;
    }
    a.push_back({L"DcTable(ents1)", tab1.entries().size() == 3 && chk});

    a.push_back({L"DcTable(ents2)",
                 tab2.entries().size() == 1 &&
                 tab2.entries()[0].type == DcTypeImage &&
                 tab2.entries()[0].elements == 32});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* dcTableTest() {
  static DcTableTest test;
  return &test;
}

TEST_NS_END
