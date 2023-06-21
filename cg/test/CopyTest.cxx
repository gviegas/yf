//
// CG
// CopyTest.cxx
//
// Copyright © 2021-2023 Gustavo C. Viegas.
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
    auto vert = dev.shader(StageVertex, "test/data/vert");
    auto frag = dev.shader(StageFragment, "test/data/frag");

    // Wsi
    auto win = WS_NS::createWindow(480, 400, name_);
    Size2 winSz{win->width(), win->height()};
    auto wsi = dev.wsi(*win);

    // Pass
    vector<AttachDesc> passClrs{{(*wsi)[0]->format(), (*wsi)[0]->samples()}};
    AttachDesc passDs{PxFormatD16Unorm, Samples1};
    auto pass = dev.pass(&passClrs, nullptr, &passDs);

    // Depth image
    auto ds = dev.image({passDs.format, {winSz, 1}, 1, passDs.samples,
                         Image::Dim2, Image::Attachment});

    // Targets
    vector<AttachImg> clrImgs{{nullptr, 0, 0}};
    AttachImg dsImg{ds.get(), 0, 0};
    vector<Target::Ptr> tgts;
    for (const auto& wi : *wsi) {
      clrImgs[0].image = wi;
      tgts.push_back(pass->target(winSz, 1, &clrImgs, nullptr, &dsImg));
    }

    // Buffer
    const float pos[3*3] = {-1.0f,  1.0f, 0.5f,
                             1.0f,  1.0f, 0.5f,
                             0.0f, -1.0f, 0.5f};

    const float tc[2*3] = {0.0f, 0.0f,
                           1.0f, 0.5f,
                           1.0f, 0.0f};

    const float xform[4*4] = {0.9f, 0.0f, 0.0f, 0.0f,
                              0.0f, 0.9f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.9f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f};

    const uint64_t align = dev.limits().minDcUniformWriteAlignedOffset;
    const uint64_t off = (sizeof pos + sizeof tc) % align ?
                         align - ((sizeof pos + sizeof tc) % align) : 0;

    const auto bufUsg = Buffer::CopySrc | Buffer::CopyDst | Buffer::Vertex |
                        Buffer::Uniform;

    auto buf = dev.buffer({4096, Buffer::Shared, bufUsg});
    buf->write(0, sizeof pos, pos);
    buf->write(sizeof pos, sizeof tc, tc);
    buf->write(sizeof pos + sizeof tc + off, sizeof xform, xform);

    // Image
    const uint8_t pixels[][4] = {
      {0, 0, 255, 255}, {0, 255, 0, 255}, {255, 0, 0, 255}
    };

    const auto imgUsg = Image::CopySrc | Image::CopyDst | Image::Sampled;

    auto img = dev.image({PxFormatRgba8Unorm, {3, 1, 1}, 1, Samples1,
                          Image::Dim2, imgUsg});
    img->write({}, {3, 1}, 0, 0, pixels);

    // DcTable
    const vector<DcEntry> dcs{{0, DcTypeUniform, 1}, {1, DcTypeImgSampler, 1}};
    auto dtb = dev.dcTable(dcs);
    dtb->allocate(1);
    dtb->write(0, 0, 0, *buf, sizeof pos + sizeof tc + off, sizeof xform);
    dtb->write(0, 1, 0, *img, 0, 0);

    // GrState
    GrState::Config config;
    config.pass = pass.get();
    config.shaders = {vert.get(), frag.get()};
    config.dcTables = {dtb.get()};
    config.vxInputs = {
      {{{0, VxFormatFlt3, 0}}, sizeof(float[3]), VxStepFnVertex},
      {{{1, VxFormatFlt2, 0}}, sizeof(float[2]), VxStepFnVertex} };
    config.topology = TopologyTriangle;
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
    Scissor sciss{{}, winSz};

    TargetOp tgtOp;
    tgtOp.colorOps.push_back({LoadOpClear, StoreOpStore});
    tgtOp.colorValues.push_back({0.0f, 0.0f, 0.0f, 1.0f});
    tgtOp.depthOp = {LoadOpClear, StoreOpStore};
    tgtOp.depthValue = 1.0f;

    while (!quit) {
      WS_NS::dispatch();

      if (key == WS_NS::KeyCodeB) {
        key = WS_NS::KeyCodeUnknown;
        // Can be GPU-private
        auto tmp = dev.buffer({buf->size(), Buffer::Private, bufUsg});
        TfEncoder enc;
        enc.copy(*tmp, 0, *buf, 0, buf->size());
        cb->encode(enc);
        cb->enqueue();
        que.submit();
        buf.reset(tmp.release());
        dtb->write(0, 0, 0, *buf, sizeof pos + sizeof tc + off, sizeof xform);
        wcout << "\n> buffer copied\n";
      } else if (key == WS_NS::KeyCodeI) {
        key = WS_NS::KeyCodeUnknown;
        auto tmp = dev.image({img->format(), {img->size(), 1}, 1, Samples1,
                              Image::Dim2, imgUsg});
        TfEncoder enc;
        enc.copy(*tmp, {}, 0, 0, *img, {}, 0, 0, img->size(), 1);
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
        enc.copy(*buf, 200, *buf, 170, cpySz);
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

      const auto next = wsi->nextImage(false);

      // Encoder
      GrEncoder enc;
      enc.setViewport(vport);
      enc.setScissor(sciss);
      enc.setTarget(*tgts[next.second], tgtOp);
      enc.setState(*state);
      enc.setDcTable(0, 0);
      enc.setVertexBuffer(*buf, 0, 0);
      enc.setVertexBuffer(*buf, sizeof pos, 1);
      enc.draw(0, 3, 0, 1);

      cb->encode(enc);
      cb->enqueue();
      que.submit();
      wsi->present(next.second);
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
