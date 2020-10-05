//
// yf
// ImageTest.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include <iostream>

#include "UnitTests.h"
#include "CGImage.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

struct ImageTest : Test {
  ImageTest() : Test(L"CGImage") {}

  Coverage run(const vector<string>& args) {
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

    Image img(CGPxFormatRgba8Unorm, 2048, 16, 1, CGSamples1);

    wcout << "\n-Image-"
          << "\nformat : " << img._format
          << "\nsize : " << img._size.width << "x" << img._size.height
          << "\nlayers : " << img._layers
          << "\nlevels : " << img._levels
          << "\nsamples : " << img._samples
          << "\nwrite() : " << img.write({0, 0}, {64, 72}, 0, 0, nullptr)
          << endl;

    // TODO
    return 60.0;
  }
};

Test* TEST_NS::imageTest() {
  static ImageTest test;
  return &test;
}
