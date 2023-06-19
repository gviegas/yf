//
// CG
// BufferTest.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
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
      Mode md_;
      UsageMask usg_;
     public:
      Buffer_(size_t sz, Mode md, UsageMask usg)
        : sz_(sz), md_(md), usg_(usg) { }
      void write(uint64_t, uint64_t, const void*) { }
      uint64_t size() const { return sz_; }
    };

    Assertions a;

    Buffer_ buf(1 << 14, Buffer::Shared, Buffer::CopySrc | Buffer::CopyDst);

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
