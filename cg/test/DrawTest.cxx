//
// CG
// DrawTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <chrono>

#include <algorithm>
#include <cassert>

#include "UnitTests.h"
#include "Device.h"
#include "Encoder.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct DrawTest : Test {
  DrawTest() : Test(L"Draw") { }

  bool draw() {
    // Get device and queue
    auto& dev = device();
    auto& que = dev.defaultQueue();

    // Create shaders
    auto vert = dev.shader(StageVertex, L"tmp/vert");
    auto frag = dev.shader(StageFragment, L"tmp/frag");

    // Create wsi
    auto win = WS_NS::createWindow(480, 400, name_);
    Size2 winSz{win->width(), win->height()};
    auto wsi = dev.wsi(win.get());
    auto wsiImgs = wsi->images();
    assert(wsiImgs.size() > 0);

    // Create pass
    vector<ColorAttach> passClrs{{wsiImgs[0]->format_, wsiImgs[0]->samples_,
                                  LoadOpLoad, StoreOpStore}};
    DepStenAttach passDs{PxFormatD16Unorm, Samples1, LoadOpLoad, StoreOpStore,
                         LoadOpDontCare, StoreOpDontCare};
    auto pass = dev.pass(&passClrs, nullptr, &passDs);

    // Create depth/stencil image
    auto ds = dev.image(passDs.format, winSz, 1, 1, passDs.samples);

    // Create a target for each wsi image
    vector<AttachImg> clrImgs{{nullptr, 0, 0}};
    AttachImg dsImg{ds.get(), 0, 0};
    vector<Target::Ptr> tgts;
    for (const auto& wi : wsiImgs) {
      clrImgs[0].image = wi;
      tgts.push_back(pass->target(winSz, 1, &clrImgs, nullptr, &dsImg));
    }

    // Create buffer and fill with data
    struct Vertex { float pos[3]; float tc[2]; };
    Vertex vdata[] = {{{-1.0f,  1.0f, 0.5f}, {0.0f, 0.0f}},
                      {{ 1.0f,  1.0f, 0.5f}, {1.0f, 0.0f}},
                      {{ 0.0f, -1.0f, 0.5f}, {0.5f, 1.0f}}};

    float mdata[] = {0.9f, 0.0f, 0.0f, 0.0f,
                     0.0f, 0.9f, 0.0f, 0.0f,
                     0.0f, 0.0f, 0.9f, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f};

    uint32_t voff = offsetof(Vertex, tc);
    uint32_t vstrd = sizeof(Vertex);

    auto buf = dev.buffer(1024);
    buf->write(0, sizeof vdata, vdata);
    buf->write(sizeof vdata, sizeof mdata, mdata);

    // Create sampling image and fill with data
    uint8_t pdata[][3] = {{0xDE, 0x20, 0x20}, {0xDF, 0x7C, 0x20},
                          {0xDC, 0x9E, 0x28}, {0xD0, 0x2A, 0x24},
                          {0xCF, 0x60, 0x3C}, {0xDF, 0x0E, 0x32}};

    auto tex = dev.image(PxFormatRgb8Unorm, {2, 3}, 1, 1, Samples1);
    tex->write({0}, {2, 3}, 0, 0, pdata);

    // Create descriptor table, allocate resources and copy data
    DcEntries dcs{{0, {DcTypeUniform, 1}},
                  {1, {DcTypeImgSampler, 1}}};
    auto dtb = dev.dcTable(dcs);
    dtb->allocate(1);
    dtb->write(0, 0, 0, *buf, sizeof vdata, sizeof mdata);
    dtb->write(0, 1, 0, *tex, 0, 0);

    // Create graphics state
    GrState::Config config{pass.get(),
                           {vert.get(), frag.get()},
                           {dtb.get()},
                           { { {{0, {VxFormatFlt3, 0}},
                                {1, {VxFormatFlt2, voff}}},
                               vstrd, VxStepFnVertex } },
                           PrimitiveTriangle,
                           PolyModeFill,
                           CullModeBack,
                           WindingCounterCw};
    auto state = dev.state(config);

    // Create command buffer
    auto cb = que.cmdBuffer();

    // Enter rendering loop
    const auto tm = chrono::system_clock::now() + chrono::seconds(5);
    while (chrono::system_clock::now() < tm) {
      // Acquire next drawable image
      Image* img;
      while (!(img = wsi->nextImage())) { }

      auto tgtIt = find_if(tgts.begin(), tgts.end(), [&](auto& tgt) {
        return tgt->colors_->front().image == img;
      });
      assert(tgtIt != tgts.end());

      // Encode commands
      GrEncoder enc;
      enc.setState(state.get());
      enc.setViewport({0.0f, 0.0f, (float)winSz.width, (float)winSz.height,
                       0.0f, 1.0f});
      enc.setScissor({{0}, winSz});
      enc.setTarget(tgtIt->get());
      enc.setDcTable(0, 0);
      enc.setVertexBuffer(buf.get(), 0);
      enc.clearColor({0.005f, 0.005f, 0.005f, 1.0f});
      enc.clearDepth(1.0f);
      enc.draw(0, 3, 0, 1);

      // Apply encoding to command buffer
      cb->encode(enc);

      // Enqueue command buffer
      cb->enqueue();

      // Submit for execution
      que.submit();

      // Present image
      wsi->present(img);
    }

    return true;
  }

  Assertions run(const vector<string>&) {
    return {{L"draw()", draw()}};
  }
};

INTERNAL_NS_END

Test* TEST_NS::drawTest() {
  static DrawTest test;
  return &test;
}
