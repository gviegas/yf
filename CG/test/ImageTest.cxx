//
// yf
// ImageTest.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "CGImage.h"
#include "Defs.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ImageTest : Test {
  ImageTest() : Test(L"CGImage") {}

  Assertions run(const vector<string>& args) {
    struct Image : CGImage {
      Image(CGPxFormat format,
            CGSize2 size,
            uint32_t layers,
            uint32_t levels,
            CGSamples samples)
        : CGImage(format, size, layers, levels, samples) {}

      CGResult write(CGOffset2, CGSize2, uint32_t, uint32_t, const void*) {
        return CGResult::Failure;
      }
    };

    Assertions a;

    Image img(CGPxFormatRgba8Unorm, 2048, 16, 1, CGSamples1);

    a.push_back({L"CGImage(CGPxFormatRgba8Unorm, 2048, 16, 1, CGSamples1)",
                 img.format == CGPxFormatRgba8Unorm &&
                 img.size == CGSize2(2048, 2048) &&
                 img.layers == 16 && img.levels == 1 &&
                 img.samples == CGSamples1 &&
                 !img.write({0, 0}, {48, 32}, 0, 0, nullptr)});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::imageTest() {
  static ImageTest test;
  return &test;
}
