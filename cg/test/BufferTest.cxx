//
// CG
// BufferTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "yf/cg/Buffer.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct BufferTest : Test {
  BufferTest() : Test(L"Buffer") {}

  Assertions run(const vector<string>&) {
    struct Buffer_ : Buffer {
      Buffer_(size_t sz) : Buffer(sz) {}

      void write(uint64_t, uint64_t, const void*) {}
    };

    Assertions a;

    Buffer_ buf(1<<12);

    a.push_back({L"Buffer(1<<12)", buf.size == (1<<12)});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::bufferTest() {
  static BufferTest test;
  return &test;
}
