//
// CG
// ImageTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "Image.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ImageTest : Test {
  ImageTest() : Test(L"Image") { }

  Assertions run(const vector<string>&) {
    struct Image_ : Image {
      Image_(PxFormat format, Size2 size, uint32_t layers, uint32_t levels,
             Samples samples)
        : Image(format, size, layers, levels, samples) { }

      void write(Offset2, Size2, uint32_t, uint32_t, const void*) { }
    };

    Assertions a;

    Image_ img(PxFormatRgba8Unorm, 2048, 16, 1, Samples1);

    a.push_back({L"Image(PxFormatRgba8Unorm, 2048, 16, 1, Samples1)",
                 img.format_ == PxFormatRgba8Unorm &&
                 img.size_ == Size2(2048, 2048) &&
                 img.layers_ == 16 && img.levels_ == 1 &&
                 img.samples_ == Samples1});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* imageTest() {
  static ImageTest test;
  return &test;
}

TEST_NS_END
