//
// CG
// CopyTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "CG.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct CopyTest : Test {
  CopyTest() : Test(L"Copy") { }

  bool copy() {
    auto& dev = device();
    auto& que = dev.defaultQueue();

    // Shaders
    auto vert = dev.shader(StageVertex, L"tmp/vert");
    auto frag = dev.shader(StageFragment, L"tmp/frag");

    // Wsi
    auto win = WS_NS::createWindow(400, 400, name_);
    Size2 winSz{win->width(), win->height()};
    auto wsi = dev.wsi(win.get());
    auto wsiImgs = wsi->images();

    // Pass
    vector<ColorAttach> passClrs{{wsiImgs[0]->format_, wsiImgs[0]->samples_,
                                  LoadOpLoad, StoreOpStore}};
    DepStenAttach passDs{PxFormatD16Unorm, Samples1, LoadOpDontCare,
                         StoreOpDontCare, LoadOpDontCare, StoreOpDontCare};
    auto pass = dev.pass(&passClrs, nullptr, &passDs);

    // Depth image
    auto ds = dev.image(passDs.format, winSz, 1, 1, passDs.samples);

    // Targets
    vector<AttachImg> clrImgs{{nullptr, 0, 0}};
    AttachImg dsImg{ds.get(), 0, 0};
    vector<Target::Ptr> tgts;
    for (const auto& wi : wsiImgs) {
      clrImgs[0].image = wi;
      tgts.push_back(pass->target(winSz, 1, &clrImgs, nullptr, &dsImg));
    }

    // Buffer
    const float pos[3*3] = {-1.0f,  1.0f, 0.5f,
                             1.0f,  1.0f, 0.5f,
                             0.0f, -1.0f, 0.5f};

    const float tc[2*3] = {0.0f, 0.0f,
                           1.0f, 0.0f,
                           0.5f, 1.0f};

    const float xform[] = {0.6f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.6f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.6f, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f};

    auto buf = dev.buffer(256);
    buf->write(0, sizeof pos, pos);
    buf->write(sizeof pos, sizeof tc, tc);
    buf->write(sizeof pos + sizeof tc, sizeof xform, xform);

    // Image
    const uint8_t pixels[][3] = {{0, 0, 255}, {0, 255, 0}, {255, 0, 0}};

    auto img = dev.image(PxFormatRgb8Unorm, {3, 1}, 1, 1, Samples1);
    img->write({0}, {3, 1}, 0, 0, pixels);

    // TODO...
    return true;
  }

  Assertions run(const vector<string>&) {
    // TODO
    return {};
  }
};

INTERNAL_NS_END

Test* TEST_NS::copyTest() {
  static CopyTest test;
  return &test;
}
