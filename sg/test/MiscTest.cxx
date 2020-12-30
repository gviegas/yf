//
// SG
// MiscTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <iostream>
#include <chrono>

#include "yf/ws/WS.h"
#include "yf/cg/CG.h"

#include "UnitTests.h"
#include "MeshImpl.h"
#include "TextureImpl.h"
#include "Matrix.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct MiscTest : public Test {
  MiscTest() : Test(L"Misc") { }

  bool misc1() {
    auto& dev = CG_NS::Device::get();
    auto& que = dev.defaultQueue();

    auto vert = dev.makeShader(CG_NS::StageVertex, L"tmp/vert");
    auto frag = dev.makeShader(CG_NS::StageFragment, L"tmp/frag");

    auto win = WS_NS::Window::make(384, 384, name_);
    const CG_NS::Size2 winSz{win->width(), win->height()};
    auto wsi = dev.makeWsi(win.get());
    auto wsiImgs = wsi->images();

    vector<CG_NS::ColorAttach> passClrs{{wsiImgs[0]->format_,
                                         wsiImgs[0]->samples_,
                                         CG_NS::LoadOpLoad,
                                         CG_NS::StoreOpStore}};
    CG_NS::DepStenAttach passDs{CG_NS::PxFormatD16Unorm, CG_NS::Samples1,
                                CG_NS::LoadOpDontCare, CG_NS::StoreOpDontCare,
                                CG_NS::LoadOpDontCare, CG_NS::StoreOpDontCare};
    auto pass = dev.makePass(&passClrs, nullptr, &passDs);

    auto ds = dev.makeImage(passDs.format, winSz, 1, 1, passDs.samples);

    vector<CG_NS::AttachImg> clrImgs{{nullptr, 0, 0}};
    CG_NS::AttachImg dsImg{ds.get(), 0, 0};
    vector<CG_NS::Target::Ptr> tgts;
    for (const auto& wi : wsiImgs) {
      clrImgs[0].image = wi;
      tgts.push_back(pass->makeTarget(winSz, 1, &clrImgs, nullptr, &dsImg));
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

    const auto mat = scale(0.5f, 0.5f, 0.5f) * translate(-0.65f, -0.65f, 0.0f);
    const size_t msize = mat.columns() * mat.rows() * sizeof(float);

    auto buf = dev.makeBuffer(1024);
    buf->write(0, sizeof vdata, vdata);
    buf->write(sizeof vdata, sizeof idata, idata);
    buf->write(sizeof vdata+sizeof idata, msize, mat.data());

    Texture tex(Texture::Bmp, L"tmp/64.bmp");

    CG_NS::DcEntries dcs{{0, {CG_NS::DcTypeUniform, 1}},
                         {1, {CG_NS::DcTypeImgSampler, 1}}};
    auto dtb = dev.makeDcTable(dcs);
    dtb->allocate(1);
    dtb->write(0, 0, 0, *buf, sizeof vdata+sizeof idata, msize);
    tex.impl().copy(*dtb, 0, 1, 0, 0, CG_NS::ImgSamplerLinear/*Basic*/);

    CG_NS::VxAttrs vattrs{{0, {CG_NS::VxFormatFlt3, 0}},
                          {1, {CG_NS::VxFormatFlt2, voff}}};
    CG_NS::VxInput vin{vattrs, vstrd, CG_NS::VxStepFnVertex};

    CG_NS::GrState::Config config{pass.get(), {vert.get(), frag.get()},
                                  {dtb.get()}, {vin}, CG_NS::PrimitiveTriangle,
                                  CG_NS::PolyModeFill, CG_NS::CullModeBack,
                                  CG_NS::WindingCounterCw};

    auto state = dev.makeState(config);

    auto cb = que.makeCmdBuffer();

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
      enc.setVertexBuffer(buf.get(), 0);
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
    auto& dev = CG_NS::Device::get();
    auto& que = dev.defaultQueue();

    auto vert = dev.makeShader(CG_NS::StageVertex, L"tmp/vert2");
    auto frag = dev.makeShader(CG_NS::StageFragment, L"tmp/frag");

    auto win = WS_NS::Window::make(384, 384, name_);
    const CG_NS::Size2 winSz{win->width(), win->height()};
    auto wsi = dev.makeWsi(win.get());
    auto wsiImgs = wsi->images();

    vector<CG_NS::ColorAttach> passClrs{{wsiImgs[0]->format_,
                                         wsiImgs[0]->samples_,
                                         CG_NS::LoadOpLoad,
                                         CG_NS::StoreOpStore}};
    CG_NS::DepStenAttach passDs{CG_NS::PxFormatD16Unorm, CG_NS::Samples1,
                                CG_NS::LoadOpDontCare, CG_NS::StoreOpDontCare,
                                CG_NS::LoadOpDontCare, CG_NS::StoreOpDontCare};
    auto pass = dev.makePass(&passClrs, nullptr, &passDs);

    auto ds = dev.makeImage(passDs.format, winSz, 1, 1, passDs.samples);

    vector<CG_NS::AttachImg> clrImgs{{nullptr, 0, 0}};
    CG_NS::AttachImg dsImg{ds.get(), 0, 0};
    vector<CG_NS::Target::Ptr> tgts;
    for (const auto& wi : wsiImgs) {
      clrImgs[0].image = wi;
      tgts.push_back(pass->makeTarget(winSz, 1, &clrImgs, nullptr, &dsImg));
    }

    const auto mat = scale(0.2f, 0.2f, 0.2f) *
                     rotate(3.141592f / 3.0f, {0.7172f, 0.7172f, 0.0f}) *
                     translate(-2.0f, 1.3f, 2.4f);
    const size_t msize = mat.columns() * mat.rows() * sizeof(float);

    auto buf = dev.makeBuffer(1024);
    buf->write(0, msize, mat.data());

    struct Vertex { float pos[3]; float tc[2]; float norm[3]; };
    Mesh mesh(Mesh::Obj, L"tmp/cube.obj");
    Texture tex(Texture::Bmp, L"tmp/cube.bmp");

    CG_NS::DcEntries dcs{{0, {CG_NS::DcTypeUniform, 1}},
                         {1, {CG_NS::DcTypeImgSampler, 1}}};
    auto dtb = dev.makeDcTable(dcs);
    dtb->allocate(1);
    dtb->write(0, 0, 0, *buf, 0, msize);
    tex.impl().copy(*dtb, 0, 1, 0, 0, CG_NS::ImgSamplerLinear/*Basic*/);

    CG_NS::VxAttrs vattrs{{0, {CG_NS::VxFormatFlt3, 0}},
                          {1, {CG_NS::VxFormatFlt2, offsetof(Vertex, tc)}},
                          {2, {CG_NS::VxFormatFlt3, offsetof(Vertex, norm)}}};
    CG_NS::VxInput vin{vattrs, sizeof(Vertex), CG_NS::VxStepFnVertex};

    CG_NS::GrState::Config config{pass.get(), {vert.get(), frag.get()},
                                  {dtb.get()}, {vin}, CG_NS::PrimitiveTriangle,
                                  CG_NS::PolyModeFill, CG_NS::CullModeBack,
                                  CG_NS::WindingCounterCw};

    auto state = dev.makeState(config);

    auto cb = que.makeCmdBuffer();

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
      mesh.impl().encodeBindings(enc);
      mesh.impl().encodeDraw(enc, 0, 1);

      cb->encode(enc);
      cb->enqueue();
      que.submit();
      wsi->present(img);
    }

    return true;
  }

  Assertions run(const vector<string>&) {
    return {{L"misc2()", misc2()}};
  }
};

INTERNAL_NS_END

Test* TEST_NS::miscTest() {
  static MiscTest test;
  return &test;
}
