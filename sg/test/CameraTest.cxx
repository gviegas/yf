//
// SG
// CameraTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "yf/cg/CG.h"
#include "yf/ws/WS.h"

#include "Test.h"
#include "MeshImpl.h"
#include "TextureImpl.h"
#include "Camera.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct Input {
  bool moveF, moveB, moveL, moveR, moveU, moveD;
  bool turnL, turnR, turnU, turnD;
  bool zoomI, zoomO;
  bool place, point, quit;
};

struct CameraTest : Test {
  CameraTest() : Test(L"Camera") { }

  static Input input;
  static constexpr float deltaM = 0.165f;
  static constexpr float deltaT = 0.025f;
  static constexpr float deltaZ = 0.035f;

  static void onKey(WS_NS::KeyCode key, WS_NS::KeyState state,
                    [[maybe_unused]] WS_NS::KeyModMask modMask) {

    const bool b = state == WS_NS::KeyStatePressed;

    switch (key) {
    case WS_NS::KeyCodeW:
      input.moveF = b;
      break;
    case WS_NS::KeyCodeS:
      input.moveB = b;
      break;
    case WS_NS::KeyCodeA:
      input.moveL= b;
      break;
    case WS_NS::KeyCodeD:
      input.moveR = b;
      break;
    case WS_NS::KeyCodeR:
      input.moveU= b;
      break;
    case WS_NS::KeyCodeF:
      input.moveD = b;
      break;
    case WS_NS::KeyCodeLeft:
      input.turnL = b;
      break;
    case WS_NS::KeyCodeRight:
      input.turnR = b;
      break;
    case WS_NS::KeyCodeUp:
      input.turnU = b;
      break;
    case WS_NS::KeyCodeDown:
      input.turnD = b;
      break;
    case WS_NS::KeyCodeE:
      input.zoomI = b;
      break;
    case WS_NS::KeyCodeQ:
      input.zoomO = b;
      break;
    case WS_NS::KeyCodeReturn:
      input.place = b;
      break;
    case WS_NS::KeyCodeSpace:
      input.point = b;
      break;
    default:
      input.quit = true;
      break;
    }
  }

  Assertions run(const vector<string>&) {
    auto& dev = CG_NS::device();
    auto& que = dev.defaultQueue();

    auto vert = dev.shader(CG_NS::StageVertex, L"tmp/vert2");
    auto frag = dev.shader(CG_NS::StageFragment, L"tmp/frag");

    auto win = WS_NS::createWindow(600, 600, name_);
    const CG_NS::Size2 winSz{win->width(), win->height()};
    auto wsi = dev.wsi(win.get());
    auto wsiImgs = wsi->images();

    Camera cam{{0.0f, 0.0f, -10.0f}, {}, static_cast<float>(winSz.width) /
                                         static_cast<float>(winSz.height)};

    vector<CG_NS::ColorAttach> passClrs{{wsiImgs[0]->format_,
                                         wsiImgs[0]->samples_,
                                         CG_NS::LoadOpLoad,
                                         CG_NS::StoreOpStore}};
    CG_NS::DepStenAttach passDs{CG_NS::PxFormatD16Unorm, CG_NS::Samples1,
                                CG_NS::LoadOpDontCare, CG_NS::StoreOpDontCare,
                                CG_NS::LoadOpDontCare, CG_NS::StoreOpDontCare};
    auto pass = dev.pass(&passClrs, nullptr, &passDs);

    auto ds = dev.image(passDs.format, winSz, 1, 1, passDs.samples);

    vector<CG_NS::AttachImg> clrImgs{{nullptr, 0, 0}};
    CG_NS::AttachImg dsImg{ds.get(), 0, 0};
    vector<CG_NS::Target::Ptr> tgts;
    for (const auto& wi : wsiImgs) {
      clrImgs[0].image = wi;
      tgts.push_back(pass->target(winSz, 1, &clrImgs, nullptr, &dsImg));
    }

    auto mat = cam.transform() * Mat4f::identity();
    const size_t msize = mat.columns() * mat.rows() * sizeof(float);

    auto buf = dev.buffer(1024);
    buf->write(0, msize, mat.data());

    Mesh mesh(Mesh::Gltf, L"tmp/cube.gltf");
    Texture tex(Texture::Png, L"tmp/cube.png");

    CG_NS::DcEntries dcs{{0, {CG_NS::DcTypeUniform, 1}},
                         {1, {CG_NS::DcTypeImgSampler, 1}}};
    auto dtb = dev.dcTable(dcs);
    dtb->allocate(1);
    dtb->write(0, 0, 0, *buf, 0, msize);
    tex.impl().copy(*dtb, 0, 1, 0, 0, nullptr);

    CG_NS::VxAttrs vpos{{0, {CG_NS::VxFormatFlt3, 0}}};
    CG_NS::VxAttrs vnorm{{2, {CG_NS::VxFormatFlt3, 0}}};
    CG_NS::VxAttrs vtc{{1, {CG_NS::VxFormatFlt2, 0}}};

    std::vector<CG_NS::VxInput> vins{
      {vpos, sizeof(float[3]), CG_NS::VxStepFnVertex},
      {vnorm, sizeof(float[3]), CG_NS::VxStepFnVertex},
      {vtc, sizeof(float[2]), CG_NS::VxStepFnVertex}};

    CG_NS::GrState::Config config{pass.get(), {vert.get(), frag.get()},
                                  {dtb.get()}, vins, CG_NS::PrimitiveTriangle,
                                  CG_NS::PolyModeFill, CG_NS::CullModeBack,
                                  CG_NS::WindingCounterCw};

    auto state = dev.state(config);

    auto cb = que.cmdBuffer();

    const CG_NS::Viewport vport{0.0f, 0.0f,
                                static_cast<float>(winSz.width),
                                static_cast<float>(winSz.height),
                                0.0f, 1.0f};
    const CG_NS::Scissor sciss{{0}, winSz};

    WS_NS::KbDelegate deleg;
    deleg.key = onKey;
    WS_NS::setDelegate(deleg);

    while (!input.quit) {
      WS_NS::dispatch();

      if (input.moveF)
        cam.moveForward(deltaM);
      if (input.moveB)
        cam.moveBackward(deltaM);
      if (input.moveL)
        cam.moveLeft(deltaM);
      if (input.moveR)
        cam.moveRight(deltaM);
      if (input.moveU)
        cam.moveUp(deltaM);
      if (input.moveD)
        cam.moveDown(deltaM);
      if (input.turnL)
        cam.turnLeft(deltaT);
      if (input.turnR)
        cam.turnRight(deltaT);
      if (input.turnU)
        cam.turnUp(deltaT);
      if (input.turnD)
        cam.turnDown(deltaT);
      if (input.zoomI)
        cam.zoomIn(deltaZ);
      if (input.zoomO)
        cam.zoomOut(deltaZ);
      if (input.place)
        cam.place({0.0f, 0.0f, -10.0f});
      if (input.point)
        cam.point({});

      mat = cam.transform() * Mat4f::identity();
      buf->write(0, msize, mat.data());
      dtb->write(0, 0, 0, *buf, 0, msize);

      CG_NS::Image *img;
      while (!(img = wsi->nextImage())) { }

      auto tgtIt = find_if(tgts.begin(), tgts.end(), [&](auto& tgt) {
        return tgt->colors_->front().image == img;
      });

      CG_NS::GrEncoder enc;
      enc.setState(state.get());
      enc.setViewport(vport);
      enc.setScissor(sciss);
      enc.setTarget(tgtIt->get());
      enc.setDcTable(0, 0);
      enc.clearColor({0.0f, 0.0f, 0.0f, 1.0f});
      enc.clearDepth(1.0f);
      mesh.impl().encodeVertexBuffer(enc, VxTypePosition, 0);
      mesh.impl().encodeVertexBuffer(enc, VxTypeNormal, 1);
      mesh.impl().encodeVertexBuffer(enc, VxTypeTexCoord0, 2);
      mesh.impl().encodeIndexBuffer(enc);
      mesh.impl().encodeDraw(enc, 0, 1);

      cb->encode(enc);
      cb->enqueue();
      que.submit();
      wsi->present(img);
    }

    return {{L"Camera()", true}};
  }
};

Input CameraTest::input{};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* cameraTest() {
  static CameraTest test;
  return &test;
}

TEST_NS_END
