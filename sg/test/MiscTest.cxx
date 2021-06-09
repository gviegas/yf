//
// SG
// MiscTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <iostream>
#include <chrono>

#include "yf/ws/WS.h"
#include "yf/cg/CG.h"

#include "Test.h"
#include "MeshImpl.h"
#include "TextureImpl.h"
#include "Renderer.h"
#include "SG.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct MiscTest : public Test {
  MiscTest() : Test(L"Misc") { }

  struct Input {
    bool moveF, moveB, moveL, moveR, moveU, moveD;
    bool turnL, turnR, turnU, turnD;
    bool zoomI, zoomO;
    bool place, point;
    bool mode, next;
    bool quit;
  };

  static Input input;
  static WS_NS::KeyCode key;
  static constexpr float deltaM = 0.215f;
  static constexpr float deltaT = 0.025f;
  static constexpr float deltaZ = 0.035f;

  static void onKey(WS_NS::KeyCode key, WS_NS::KeyState state,
                    [[maybe_unused]] WS_NS::KeyModMask modMask) {

    const bool b = state == WS_NS::KeyStatePressed;
    MiscTest::key = key;

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
    case WS_NS::KeyCodeM:
      input.mode = b;
      break;
    case WS_NS::KeyCodeN:
      input.next = b;
      break;
    case WS_NS::KeyCodeEsc:
      input.quit = b;
      break;
    default:
      break;
    }
  }

  bool misc1() {
    auto& dev = CG_NS::device();
    auto& que = dev.defaultQueue();

    auto vert = dev.shader(CG_NS::StageVertex, L"tmp/vert");
    auto frag = dev.shader(CG_NS::StageFragment, L"tmp/frag");

    auto win = WS_NS::createWindow(384, 384, name_);
    const CG_NS::Size2 winSz{win->width(), win->height()};
    auto wsi = dev.wsi(win.get());
    auto wsiImgs = wsi->images();

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

    struct Vertex {
      float pos[3];
      float tc[2];
    };
    const uint32_t voff = offsetof(Vertex, tc);
    const uint32_t vstrd = sizeof(Vertex);

    const Vertex vdata[]{{{-1.0f, -1.0f, 0.5f}, {0.0f, 1.0f}},
                         {{-1.0f,  1.0f, 0.5f}, {0.0f, 0.0f}},
                         {{ 1.0f,  1.0f, 0.5f}, {1.0f, 0.0f}},
                         {{ 1.0f, -1.0f, 0.5f}, {1.0f, 1.0f}}};

    const uint16_t idata[]{0, 1, 2, 0, 2, 3};

    const auto mat = scale(0.5f, -0.5f, 0.5f) * translate(-0.65f, -0.65f, 0.0f);
    const size_t msize = mat.columns() * mat.rows() * sizeof(float);

    auto buf = dev.buffer(1024);
    buf->write(0, sizeof vdata, vdata);
    buf->write(sizeof vdata, sizeof idata, idata);
    buf->write(sizeof vdata+sizeof idata, msize, mat.data());

    Texture tex(Texture::Png, L"tmp/192.png");

    CG_NS::DcEntries dcs{{0, {CG_NS::DcTypeUniform, 1}},
                         {1, {CG_NS::DcTypeImgSampler, 1}}};
    auto dtb = dev.dcTable(dcs);
    dtb->allocate(1);
    dtb->write(0, 0, 0, *buf, sizeof vdata+sizeof idata, msize);
    tex.impl().copy(*dtb, 0, 1, 0, 0, nullptr);

    CG_NS::VxAttrs vattrs{{0, {CG_NS::VxFormatFlt3, 0}},
                          {1, {CG_NS::VxFormatFlt2, voff}}};
    CG_NS::VxInput vin{vattrs, vstrd, CG_NS::VxStepFnVertex};

    CG_NS::GrState::Config config{pass.get(), {vert.get(), frag.get()},
                                  {dtb.get()}, {vin}, CG_NS::PrimitiveTriangle,
                                  CG_NS::PolyModeFill, CG_NS::CullModeNone,
                                  CG_NS::WindingCounterCw};

    auto state = dev.state(config);

    auto cb = que.cmdBuffer();

    const CG_NS::Viewport vport{0.0f, 0.0f,
                                static_cast<float>(winSz.width),
                                static_cast<float>(winSz.height),
                                0.0f, 1.0f};
    const CG_NS::Scissor sciss{{0}, winSz};

    const auto tm = chrono::system_clock::now() + chrono::seconds(5);
    while (chrono::system_clock::now() < tm) {
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
      enc.setVertexBuffer(buf.get(), 0, 0);
      enc.setIndexBuffer(buf.get(), sizeof vdata, CG_NS::IndexTypeU16);
      enc.clearColor({0.0f, 0.0f, 0.0f, 1.0f});
      enc.clearDepth(1.0f);
      enc.drawIndexed(0, 6);

      cb->encode(enc);
      cb->enqueue();
      que.submit();
      wsi->present(img);
    }

    return true;
  }

  bool misc2() {
    auto& dev = CG_NS::device();
    auto& que = dev.defaultQueue();

    auto vert = dev.shader(CG_NS::StageVertex, L"tmp/vert2");
    auto frag = dev.shader(CG_NS::StageFragment, L"tmp/frag");

    auto win = WS_NS::createWindow(384, 384, name_);
    const CG_NS::Size2 winSz{win->width(), win->height()};
    auto wsi = dev.wsi(win.get());
    auto wsiImgs = wsi->images();

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

    const auto mat = scale(0.2f, 0.2f, 0.2f) *
                     rotate(3.141592f / 3.0f, {0.7071f, 0.7071f, 0.0f}) *
                     translate(-2.0f, 1.3f, 2.4f);
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

    const auto tm = chrono::system_clock::now() + chrono::seconds(5);
    while (chrono::system_clock::now() < tm) {
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

    return true;
  }

  bool misc3() {
    auto& dev = CG_NS::device();

    auto win = WS_NS::createWindow(384, 384, name_);
    const CG_NS::Size2 winSz{win->width(), win->height()};
    auto wsi = dev.wsi(win.get());
    auto wsiImgs = wsi->images();

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

    Mesh mesh(Mesh::Gltf, L"tmp/cube.gltf");
    Texture tex(Texture::Png, L"tmp/cube.png");
    Material matl{{tex, {}, {}, 1.0f, 1.0f}, {}, {}, {}};

    Model mdl;
    mdl.setMesh(mesh);
    mdl.setMaterial(matl);

    Scene scn;
    scn.insert(mdl);

    Renderer rend;

    const auto tm = chrono::system_clock::now() + chrono::seconds(5);
    auto angle = 0.0f;
    auto scale = 1.0f;
    while (chrono::system_clock::now() < tm) {
      CG_NS::Image *img;
      while (!(img = wsi->nextImage())) { }

      auto tgtIt = find_if(tgts.begin(), tgts.end(), [&](auto& tgt) {
        return tgt->colors_->front().image == img;
      });

      mdl.transform() = SG_NS::scale(scale, scale, scale) * rotateZ(angle);
      angle += 3.14159265359f * 0.01f;
      scale += 0.025f;

      rend.render(scn, **tgtIt);
      wsi->present(img);
    }

    return true;
  }

#define SG_PRINTMAT(mat) do { \
  for (size_t i = 0; i < mat.rows(); ++i) { \
    wcout << "\n    "; \
    for (size_t j = 0; j < mat.columns(); ++j) \
      wcout << mat[j][i] << "\t"; \
  } \
  wcout << endl; \
} while (false)

  bool misc4() {
    //Collection coll(L"tmp/fullscene2.gltf");
    //Collection coll(L"tmp/fullscene.gltf");
    Collection coll;
    //coll.load(L"tmp/animation.gltf");
    //coll.load(L"tmp/animation2.gltf");
    coll.load(L"tmp/animation3.gltf");
    //coll.load(L"tmp/cube.gltf");

    // Print
    wcout << "\nCollection:";
    wcout << "\n Scenes: #" << coll.scenes().size();
    for (const auto& scn: coll.scenes()) {
      wcout << "\n  Scene `" << scn->name() << "`:";
      wcout << "\n   children: ";
      for (const auto& chd : scn->children())
        wcout << "\n    `" << chd->name() << "`";
    }

    wcout << "\n Nodes: #" << coll.nodes().size();
    for (const auto& nd: coll.nodes()) {
      wcout << "\n  Node `" << nd->name() << "`:";
      if (nd->parent())
        wcout << "\n   parent: `" << nd->parent()->name() << "`";
      else
        wcout << "\n   (no parent)";
      if (nd->isLeaf()) {
        wcout << "\n   (no children)";
      } else {
        wcout << "\n   children: ";
        for (const auto& chd : nd->children())
          wcout << "\n    `" << chd->name() << "`";
      }
    }

    wcout << "\n Meshes: #" << coll.meshes().size();
    wcout << "\n Textures: #" << coll.textures().size();

    wcout << "\n Materials: #" << coll.materials().size();
    for (const auto& matl : coll.materials()) {
      wcout << "\n  Material:"
            << "\n   pbrmr:"
            << "\n    colorTex: " << (matl.pbrmr().colorTex ? 'y':'n')
            << "\n    colorFac: [" << matl.pbrmr().colorFac[0] << ", "
                                   << matl.pbrmr().colorFac[1] << ", "
                                   << matl.pbrmr().colorFac[2] << ", "
                                   << matl.pbrmr().colorFac[3] << "]"
            << "\n    metalRoughTex: " << (matl.pbrmr().metalRoughTex ? 'y':'n')
            << "\n    metallic: " << matl.pbrmr().metallic
            << "\n    roughness: " << matl.pbrmr().roughness
            << "\n   normal:"
            << "\n    texture: " << (matl.normal().texture ? 'y':'n')
            << "\n    scale: " << matl.normal().scale
            << "\n   occlusion:"
            << "\n    texture: " << (matl.occlusion().texture ? 'y':'n')
            << "\n    strength: " << matl.occlusion().strength
            << "\n   emissive:"
            << "\n    texture: " << (matl.emissive().texture ? 'y' : 'n')
            << "\n    factor: [" << matl.emissive().factor[0] << ", "
                                 << matl.emissive().factor[1] << ", "
                                 << matl.emissive().factor[2] << "]";
    }

    wcout << "\n Skins: #" << coll.skins().size();
    for (const auto& sk : coll.skins()) {
      wcout << "\n  Skin:"
            << "\n   joints: #" <<  sk.joints().size();
      for (const auto& jt : sk.joints()) {
        wcout << "\n\n   `" << jt->name() << "`";
        SG_PRINTMAT(jt->transform());
      }
      wcout << "\n\n   inverseBind: #" << sk.inverseBind().size();
      wcout << "\n";
      for (const auto& ib : sk.inverseBind())
        SG_PRINTMAT(ib);
    }

    wcout << "\n Animations: #" << coll.animations().size();
    for (const auto& an: coll.animations()) {
      wcout << "\n  Animation `" << an.name() << "`:"
            << "\n   actions: #" << an.actions().size();
      for (const auto& act: an.actions())
        wcout << "\n    `" << act.target->name() << "`|"
                           << act.type << "|" << act.method << "|"
                           << act.input << "|" << act.output;
      wcout << "\n   inputs: #" << an.inputs().size();
      for (const auto& in : an.inputs()) {
        wcout << "\n    *";
        for (const auto& k : in)
          wcout << "\n     " << k;
      }
      wcout << "\n   outT: #" << an.outT().size();
      for (const auto& t : an.outT()) {
        wcout << "\n    *";
        for (const auto& v : t)
          wcout << "\n     [" << v[0] << ", " << v[1] << ", " << v[2] << "]";
      }
      wcout << "\n   outR: #" << an.outR().size();
      for (const auto& r : an.outR()) {
        wcout << "\n    *";
        for (const auto& q : r)
          wcout << "\n     (" << q.r() << ", [" << q.v()[0] << ", "
                              << q.v()[1] << ", " << q.v()[2] << "])";
      }
      wcout << "\n   outS: #" << an.outS().size();
      for (const auto& s : an.outS()) {
        wcout << "\n    *";
        for (const auto& v : s)
          wcout << "\n     [" << v[0] << ", " << v[1] << ", " << v[2] << "]";
      }
    }

    wcout << endl;

#if 0
    return true;
#endif

    // Render
    auto win = WS_NS::createWindow(480, 400, L"Misc 4");
    View view(win.get());

    WS_NS::onKbKey(onKey);

    auto scn = coll.scenes().front().get();
    scn->camera().place({10.0f, 10.0f, 10.0f});
    scn->camera().point({});

    Node* obj = nullptr;
    for (auto& nd : coll.nodes()) {
      if (nd->name() == L"Cube") {
        obj = nd.get();
        break;
      }
    }

    bool camMode = true;
    bool isPlaying = false;

    view.loop(*scn, 60, [&](auto elapsedTime) {
      if (input.quit)
        return false;

      auto& cam = scn->camera();

      if (input.mode) {
        camMode = !camMode;
        input.mode = false;
      }

      if (camMode || !obj) {
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
          cam.place({0.0f, 0.0f, 20.0f});
        if (input.point)
          cam.point({});
      } else {
        auto& xform = obj->transform();
        if (input.moveF)
          xform[3] += {0.0f, 0.0f, 0.1f, 0.0f};
        if (input.moveB)
          xform[3] += {0.0f, 0.0f, -0.1f, 0.0f};
        if (input.moveL)
          xform[3] += {-0.1f, 0.0f, 0.0f, 0.0f};
        if (input.moveR)
          xform[3] += {0.1f, 0.0f, 0.0f, 0.0f};
        if (input.moveU)
          xform[3] += {0.0f, 0.1f, 0.0f, 0.0f};
        if (input.moveD)
          xform[3] += {0.0f, -0.1f, 0.0f, 0.0f};
        if (input.turnL)
          xform *= rotate(rotateQY(0.1f));
        if (input.turnR)
          xform *= rotate(rotateQY(-0.1f));
        if (input.turnU)
          xform *= rotate(rotateQX(-0.1f));
        if (input.turnD)
          xform *= rotate(rotateQX(0.1f));
        if (input.place)
          xform[3] = {0.0f, 0.0f, 0.0f, 1.0f};
      }

      if (key == WS_NS::KeyCodeZ) {
        if (!coll.animations().empty())
          isPlaying = true;
      } else if (key == WS_NS::KeyCodeX) {
        if (!coll.animations().empty()) {
          isPlaying = false;
          coll.animations().back().stop();
        }
      }

      if (isPlaying)
        wcout << "\n completed ? "
              << (coll.animations().back().play(elapsedTime) ? "no" : "yes");

      key = WS_NS::KeyCodeUnknown;
      return true;
    });

    return true;
  }

  bool misc5() {
    Collection coll;
    coll.load(L"tmp/cube.gltf");

    // Dup.
    Node* obj = nullptr;
    for (auto& nd : coll.nodes()) {
      if (nd->name() == L"Cube") {
        Model* mdl = dynamic_cast<Model*>(nd.get());
        auto mesh = mdl->mesh();
        auto matl = mdl->material();
        Skin skin{};
        obj = new Model(mesh, matl, skin);
        nd->parent()->insert(*obj);
        obj->name() = L"Cube2";
        obj->transform() = translate(2.0f, 0.0f, 0.0f);
        break;
      }
    }
    if (obj)
      coll.nodes().push_back(unique_ptr<Model>(static_cast<Model*>(obj)));

    // Render
    auto win = WS_NS::createWindow(640, 480, L"Misc 5");
    View view(win.get());

    WS_NS::onKbKey(onKey);

    auto scn = coll.scenes().front().get();
    scn->camera().place({10.0f, 10.0f, 10.0f});
    scn->camera().point({});

    bool camMode = true;
    bool isPlaying = false;

    view.loop(*scn, 60, [&](auto elapsedTime) {
      if (input.quit)
        return false;

      auto& cam = scn->camera();

      if (input.mode) {
        camMode = !camMode;
        input.mode = false;
      }

      if (input.next && obj) {
        wstring name = obj->name() == L"Cube" ? L"Cube2" : L"Cube";
        for (auto& nd : coll.nodes()) {
          if (nd->name() == name) {
            obj = nd.get();
            break;
          }
        }
        input.next = false;
      }

      if (camMode || !obj) {
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
          cam.place({0.0f, 0.0f, 20.0f});
        if (input.point)
          cam.point({});
      } else {
        auto& xform = obj->transform();
        if (input.moveF)
          xform[3] += {0.0f, 0.0f, 0.1f, 0.0f};
        if (input.moveB)
          xform[3] += {0.0f, 0.0f, -0.1f, 0.0f};
        if (input.moveL)
          xform[3] += {-0.1f, 0.0f, 0.0f, 0.0f};
        if (input.moveR)
          xform[3] += {0.1f, 0.0f, 0.0f, 0.0f};
        if (input.moveU)
          xform[3] += {0.0f, 0.1f, 0.0f, 0.0f};
        if (input.moveD)
          xform[3] += {0.0f, -0.1f, 0.0f, 0.0f};
        if (input.turnL)
          xform *= rotate(rotateQY(0.1f));
        if (input.turnR)
          xform *= rotate(rotateQY(-0.1f));
        if (input.turnU)
          xform *= rotate(rotateQX(-0.1f));
        if (input.turnD)
          xform *= rotate(rotateQX(0.1f));
        if (input.place)
          xform[3] = {0.0f, 0.0f, 0.0f, 1.0f};
      }

      if (key == WS_NS::KeyCodeZ) {
        if (!coll.animations().empty())
          isPlaying = true;
      } else if (key == WS_NS::KeyCodeX) {
        if (!coll.animations().empty()) {
          isPlaying = false;
          coll.animations().back().stop();
        }
      }

      if (isPlaying)
        wcout << "\n completed ? "
              << (coll.animations().back().play(elapsedTime) ? "no" : "yes");

      key = WS_NS::KeyCodeUnknown;
      return true;
    });

    return true;
  }

  Assertions run(const vector<string>&) {
    return {{L"misc5()", misc5()}};
  }
};

MiscTest::Input MiscTest::input;
WS_NS::KeyCode MiscTest::key = WS_NS::KeyCodeUnknown;

INTERNAL_NS_END

TEST_NS_BEGIN

Test* miscTest() {
  static MiscTest test;
  return &test;
}

TEST_NS_END
