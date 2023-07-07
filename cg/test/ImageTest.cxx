//
// CG
// ImageTest.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
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
     public:
      Image_(const Desc& desc) : Image(desc) { }
      ImgView::Ptr view(const ImgView::Desc&) { return nullptr; }
      void write(uint32_t, Origin3, uint32_t, const void*, Size3, uint32_t,
                 uint32_t) { }
    };

    Assertions a;

    Image_ img({
      Format::Rgba8Unorm,
      {2048, 2048, 16},
      1,
      Samples1,
      Image::Dim2,
      Image::CopySrc | Image::CopyDst | Image::Sampled
    });

    a.push_back({L"Image(Format::Rgba8Unorm, 2048, 16, 1, Samples1)",
                 img.format() == Format::Rgba8Unorm &&
                 img.size() == Size3(2048, 2048, 16) &&
                 img.levels() == 1 && img.samples() == Samples1 &&
                 img.dimension() == Image::Dim2 &&
                 img.usageMask() == (Image::CopyDst | Image::Sampled |
                                     Image::CopySrc)});

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
