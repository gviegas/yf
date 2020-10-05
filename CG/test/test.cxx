//
// yf
// test.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include <iostream>
#include <cassert>

#include "CGBuffer.h"
#include "CGImage.h"

using namespace std;
using namespace YF_NS;

namespace {

void testResult() {
  CGResult ok1(CGResult::Success);
  CGResult ok2(CGResult::Success);
  CGResult nok(CGResult::Failure);

  wcout << "\n-Result-"
        << "\nok1 == ok2 : " << (ok1 == ok2)
        << "\nok1 : " << ok1
        << "\n!ok1 : " << !ok1
        << "\nok1 == nok : " << (ok1 == nok)
        << "\nok1 != nok : " << (ok1 != nok)
        << "\nnok : " << nok
        << "\n!nok : " << !nok
        << endl;
}

void testBuffer() {
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
}

void testImage() {
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
}

} // ns

int main(int argc, char* argv[]) {
  wcout << "[yf-CG] Test\n------------\n\n";
  for (int i = 0; i < argc; ++i)
    wcout << argv[i] << " ";
  wcout << endl;

  testResult();
  testBuffer();
  testImage();

  wcout << "\n-----------\nEnd of test\n";
}
