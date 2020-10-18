//
// yf
// BufferTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "CGBuffer.h"
#include "Defs.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct BufferTest : Test {
  BufferTest() : Test(L"CGBuffer") {}

  Assertions run(const vector<string>&) {
    struct Buffer : CGBuffer {
      Buffer(size_t sz) : CGBuffer(sz) {}
      CGResult write(uint64_t, uint64_t, const void*) {
        return CGResult::Failure;
      }
    };

    Assertions a;

    Buffer buf(1<<12);

    a.push_back({L"CGBuffer(1<<12)",
                 buf.size == (1<<12) && !buf.write(0, 0, nullptr)});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::bufferTest() {
  static BufferTest test;
  return &test;
}
