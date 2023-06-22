//
// CG
// BufferTest.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#include "Test.h"
#include "Buffer.h"
#include "Device.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct BufferTest : Test {
  BufferTest() : Test(L"Buffer") { }

  void runMock(Assertions& a) {
    class Buffer_ : public Buffer {
      uint64_t sz_;
      Mode md_;
      UsageMask usg_;
     public:
      Buffer_(size_t sz, Mode md, UsageMask usg)
        : sz_(sz), md_(md), usg_(usg) { }
      void write(uint64_t, const void*, uint64_t) { }
      uint64_t size() const { return sz_; }
      Mode mode() const { return md_; }
      UsageMask usageMask() const { return usg_; }
    };

    Buffer_ buf(65536, Buffer::Shared, Buffer::CopySrc | Buffer::CopyDst);

    a.push_back({L"Buffer()",
                 buf.size() == 65536 &&
                 buf.mode() == Buffer::Shared &&
                 buf.usageMask() == (Buffer::CopySrc | Buffer::CopyDst)});
  }

  void runImpl(Assertions& a) {
    const uint64_t sizes[]{1, 256, 1024, 1023, 10'000, 1 << 20, 4 << 20};
    const Buffer::Mode modes[]{Buffer::Shared, Buffer::Private};
    const Buffer::UsageMask usageMasks[]{
      Buffer::CopySrc | Buffer::CopyDst,
      Buffer::Vertex,
      Buffer::Index | Buffer::Vertex,
      Buffer::Indirect | Buffer::CopyDst,
      Buffer::Uniform,
      Buffer::CopySrc | Buffer::Uniform,
      Buffer::Storage,
      Buffer::CopySrc | Buffer::Storage,
      Buffer::Query | Buffer::CopySrc,
      Buffer::CopySrc | Buffer::CopyDst | Buffer::Vertex | Buffer::Index |
        Buffer::Indirect | Buffer::Uniform | Buffer::Storage | Buffer::Query
    };

    auto check = true;

    for (auto& sz : sizes) {
      for (auto& md : modes) {
        for (auto& um : usageMasks) {
          auto buf = device().buffer({sz, md, um});
          check &= buf->size() == sz && buf->mode() == md &&
                   buf->usageMask() == um;
        }
      }
    }

    if (check) {
      const size_t inds[3][3]{{1, 0, 0}, {2, 1, 1}, {3, 0, 2}};
      Buffer::Ptr bufs[3];
      for (int i = 0; i < 3; i++)
        bufs[i] = device().buffer({sizes[inds[i][0]],
                                   modes[inds[i][1]],
                                   usageMasks[inds[i][2]]});
      for (int i = 0; i < 3; i++)
        check &= bufs[i]->size() == sizes[inds[i][0]] &&
                 bufs[i]->mode() == modes[inds[i][1]] &&
                 bufs[i]->usageMask() == usageMasks[inds[i][2]];
    }

    a.push_back({L"device().buffer()", check});
  }

  Assertions run(const std::vector<string>&) {
    Assertions a;
    runMock(a);
    runImpl(a);
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
