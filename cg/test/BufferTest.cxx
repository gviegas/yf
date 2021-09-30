//
// CG
// BufferTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "Buffer.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct BufferTest : Test {
  BufferTest() : Test(L"Buffer") { }

  Assertions run(const vector<string>&) {
    class Buffer_ : public Buffer {
      uint64_t sz_;
     public:
      Buffer_(size_t sz) : sz_(sz) { }
      void write(uint64_t, uint64_t, const void*) { }
      uint64_t size() const { return sz_; }
    };

    Assertions a;

    Buffer_ buf(1<<14);

    a.push_back({L"Buffer(1<<14)", buf.size() == (1<<14)});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* bufferTest() {
  static BufferTest test;
  return &test;
}

TEST_NS_END
