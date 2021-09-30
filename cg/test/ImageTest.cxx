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
    class Image_ : public Image {
      PxFormat format_;
      Size2 size_;
      uint32_t layers_;
      uint32_t levels_;
      Samples samples_;
     public:
      Image_(PxFormat format, Size2 size, uint32_t layers, uint32_t levels,
             Samples samples)
        : format_(format), size_(size), layers_(layers), levels_(levels),
          samples_(samples) { }

      void write(Offset2, Size2, uint32_t, uint32_t, const void*) { }
      PxFormat format() const { return format_; }
      Size2 size() const { return size_; }
      uint32_t layers() const { return layers_; }
      uint32_t levels() const { return levels_; }
      Samples samples() const { return samples_; }
    };

    Assertions a;

    Image_ img(PxFormatRgba8Unorm, 2048, 16, 1, Samples1);

    a.push_back({L"Image(PxFormatRgba8Unorm, 2048, 16, 1, Samples1)",
                 img.format() == PxFormatRgba8Unorm &&
                 img.size() == Size2(2048, 2048) &&
                 img.layers() == 16 && img.levels() == 1 &&
                 img.samples() == Samples1});

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
