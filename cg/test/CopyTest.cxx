//
// CG
// CopyTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "Test.h"
#include "CG.h"
#include "yf/ws/WS.h"

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
    auto win = WS_NS::createWindow(480, 320, name_);
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

    // DcTable
    DcEntries dcs{{0, {DcTypeUniform, 1}}, {1, {DcTypeImgSampler, 1}}};
    auto dtb = dev.dcTable(dcs);
    dtb->allocate(1);
    dtb->write(0, 0, 0, *buf, sizeof pos + sizeof tc, sizeof xform);
    dtb->write(0, 1, 0, *img, 0, 0);

    // GrState
    GrState::Config config;
    config.pass = pass.get();
    config.shaders = {vert.get(), frag.get()};
    config.dcTables = {dtb.get()};
    config.vxInputs = {
      {{{0, {VxFormatFlt3, 0}}}, sizeof(float[3]), VxStepFnVertex},
      {{{1, {VxFormatFlt2, 0}}}, sizeof(float[2]), VxStepFnVertex} };
    config.primitive = PrimitiveTriangle;
    config.polyMode = PolyModeFill;
    config.cullMode = CullModeNone;
    config.winding = WindingCounterCw;

    auto state = dev.state(config);

    // CmdBuffer
    auto cb = que.cmdBuffer();

    // Render
    auto quit = false;
    auto key = WS_NS::KeyCodeUnknown;

    WS_NS::onKbKey([&](WS_NS::KeyCode k, WS_NS::KeyState s, auto) {
      if (s == WS_NS::KeyStatePressed)
        key = k;
    });

    Viewport vport{0.0f, 0.0f, static_cast<float>(winSz.width),
                   static_cast<float>(winSz.height), 0.0f, 1.0f};
    Scissor sciss{{0}, winSz};

    while (!quit) {
      WS_NS::dispatch();

      if (key == WS_NS::KeyCodeB) {
        key = WS_NS::KeyCodeUnknown;
        auto tmp = dev.buffer(buf->size_);
        TfEncoder enc;
        enc.copy(tmp.get(), 0, buf.get(), 0, buf->size_);
        cb->encode(enc);
        cb->enqueue();
        que.submit();
        buf.reset(tmp.release());
        dtb->write(0, 0, 0, *buf, sizeof pos + sizeof tc, sizeof xform);
        wcout << "\n> buffer copied\n";
      } else if (key == WS_NS::KeyCodeI) {
        key = WS_NS::KeyCodeUnknown;
        auto tmp = dev.image(img->format_, img->size_, 1, 1, Samples1);
        TfEncoder enc;
        enc.copy(tmp.get(), {0}, 0, 0, img.get(), {0}, 0, 0, img->size_, 1);
        cb->encode(enc);
        cb->enqueue();
        que.submit();
        img.reset(tmp.release());
        dtb->write(0, 1, 0, *img, 0, 0);
        wcout << "\n> image copied\n";
      } else if (key == WS_NS::KeyCodeO) {
        static uint64_t cpySz = 31;
        key = WS_NS::KeyCodeUnknown;
        TfEncoder enc;
        enc.copy(buf.get(), 200, buf.get(), 170, cpySz);
        try {
          cb->encode(enc);
          cb->enqueue();
          que.submit();
          wcout << "\n> buffer memory do not overlap - copied\n";
        } catch (invalid_argument& e) {
          wcout << "\n> buffer memory do overlap - not copied\n";
          cpySz--;
        }
      } else if (key == WS_NS::KeyCodeEsc) {
        quit = true;
        break;
      }

      Image* next = wsi->nextImage(false);
      auto tgtIt = find_if(tgts.begin(), tgts.end(), [&](auto& tgt) {
        return tgt->colors_->front().image == next;
      });

      // Encoder
      GrEncoder enc;
      enc.setState(state.get());
      enc.setViewport(vport);
      enc.setScissor(sciss);
      enc.setTarget(tgtIt->get());
      enc.setDcTable(0, 0);
      enc.setVertexBuffer(buf.get(), 0, 0);
      enc.setVertexBuffer(buf.get(), sizeof pos, 1);
      enc.clearColor({0.0f, 0.0f, 0.0f, 1.0f});
      enc.clearDepth(1.0f);
      enc.draw(0, 3, 0, 1);

      cb->encode(enc);
      cb->enqueue();
      que.submit();
      wsi->present(next);
    }

    return true;
  }

  Assertions run(const vector<string>&) {
    return {{L"copy()", copy()}};
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* copyTest() {
  static CopyTest test;
  return &test;
}

TEST_NS_END
