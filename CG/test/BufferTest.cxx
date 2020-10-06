//
// yf
// BufferTest.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include <iostream>

#include "UnitTests.h"
#include "CGBuffer.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

struct BufferTest : Test {
  BufferTest() : Test(L"CGBuffer") {}

  Coverage run(const vector<string>& args) {
    struct Buffer : CGBuffer {
      Buffer(size_t sz) : CGBuffer(sz) {}
      CGResult write(uint64_t, uint64_t, const void*) {
        return CGResult::Failure;
      }
    };

    Buffer buf(1<<12);

    wcout << "\n-Buffer-"
          << "\nsize : " << buf._size
          << "\nwrite() : " << buf.write(0, 0, nullptr)
          << endl;

    // TODO
    return 0.8;
  }
};

Test* TEST_NS::bufferTest() {
  static BufferTest test;
  return &test;
}
