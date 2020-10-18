//
// cg
// ImageTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "yf/cg/Image.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ImageTest : Test {
  ImageTest() : Test(L"Image") {}

  Assertions run(const vector<string>&) {
    struct Image_ : Image {
      Image_(PxFormat format,
             Size2 size,
             uint32_t layers,
             uint32_t levels,
             Samples samples)
             : Image(format, size, layers, levels, samples) {}

      Result write(Offset2, Size2, uint32_t, uint32_t, const void*) {
        return Result::Failure;
      }
    };

    Assertions a;

    Image_ img(PxFormatRgba8Unorm, 2048, 16, 1, Samples1);

    a.push_back({L"Image(PxFormatRgba8Unorm, 2048, 16, 1, Samples1)",
                 img.format == PxFormatRgba8Unorm &&
                 img.size == Size2(2048, 2048) &&
                 img.layers == 16 && img.levels == 1 &&
                 img.samples == Samples1 &&
                 !img.write({0, 0}, {48, 32}, 0, 0, nullptr)});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::imageTest() {
  static ImageTest test;
  return &test;
}
