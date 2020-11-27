//
// CG
// DrawTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
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
    auto& dev = Device::get();
    auto& que = dev.defaultQueue();

    // Create shaders
    auto vert = dev.makeShader(StageVertex, L"tmp/vert");
    auto frag = dev.makeShader(StageFragment, L"tmp/frag");

    // Create wsi
    auto win = WS_NS::Window::make(400, 400, name_, WS_NS::Window::Resizable);
    Size2 winSz{win->width(), win->height()};
    auto wsi = dev.makeWsi(win.get());
    auto wsiImgs = wsi->images();
    assert(wsiImgs.size() > 0);

    // Create pass
    vector<ColorAttach> passClrs{{wsiImgs[0]->format_, wsiImgs[0]->samples_,
                                  LoadOpLoad, StoreOpStore}};
    DepStenAttach passDs{PxFormatD16Unorm, Samples1, LoadOpLoad, StoreOpStore,
                         LoadOpDontCare, StoreOpDontCare};
    auto pass = dev.makePass(&passClrs, nullptr, &passDs);

    // Create depth/stencil image
    auto ds = dev.makeImage(passDs.format, winSz, 1, 1, passDs.samples);

    // Create a target for each wsi image
    vector<AttachImg> clrImgs{{nullptr, 0, 0}};
    AttachImg dsImg{ds.get(), 0, 0};
    vector<Target::Ptr> tgts;
    for (const auto& wi : wsiImgs) {
      clrImgs[0].image = wi;
      tgts.push_back(pass->makeTarget(winSz, 1, &clrImgs, nullptr, &dsImg));
    }

    // Create buffer and fill with vertex data
    auto buf = dev.makeBuffer(512);
    float vertData[] = {-1.0f,  1.0f, 0.5f,
                         1.0f,  1.0f, 0.5f,
                         0.0f, -1.0f, 0.5f};
    buf->write(0, sizeof vertData, vertData);
    uint32_t strd = sizeof vertData / 3;

    // Create graphics state
    GrState::Config config{pass.get(),
                           {vert.get(), frag.get()},
                           {},
                           {
                             { {{0, {VxFormatFlt3, 0}}}, strd, VxStepFnVertex }
                           },
                           PrimitiveTriangle,
                           PolyModeFill,
                           CullModeNone,
                           WindingCounterCw};
    auto state = dev.makeState(config);

    // Create command buffer
    auto cb = que.makeCmdBuffer();

    // Render loop
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
      enc.setViewport({0.0f, 0.0f, 400.0f, 400.0f, 0.0f, 1.0f});
      enc.setScissor({{0, 0}, winSz});
      enc.setTarget(tgtIt->get());
      enc.setVertexBuffer(buf.get(), 0);
      enc.clearColor({1.0f, 0.0f, 0.0f, 1.0f});
      enc.clearDepth(1.0f);
      enc.draw(0, 3, 0, 1);

      // Apply encodings to command buffer
      cb->encode(enc);

      // Enqueue command buffer for execution
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
