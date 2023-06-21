//
// CG
// EncoderTest.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#include "Test.h"
#include "Encoder.h"
#include "Device.h"
#include "Cmd.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct EncoderTest : Test {
  EncoderTest() : Test(L"Encoder") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    const vector<AttachDesc> desc{{PxFormatRgba8Unorm, Samples1}};
    auto pass = device().pass(&desc, nullptr, nullptr);
    auto img = device().image({PxFormatRgba8Unorm, {480, 300, 1}, 1, Samples1,
                               Image::Dim2, Image::CopySrc | Image::CopyDst |
                                            Image::Attachment});
    const vector<AttachImg> att{{img.get(), 0, 0}};
    auto tgt = pass->target({480, 300}, 1, &att, nullptr, nullptr);

    auto vert = device().shader(StageVertex, "test/data/vert");
    auto dtb = device().dcTable({{0, DcTypeUniform, 1}});
    const VxInput vxIn{
      {{0, VxFormatFlt3, 0}, {1, VxFormatFlt2, 12}}, 20, VxStepFnVertex
    };
    const GrState::Config gconf{
      pass.get(), {vert.get()}, {dtb.get()}, {vxIn},
      TopologyTriangle, PolyModeFill, CullModeBack, WindingCounterCw
    };
    auto gst = device().state(gconf);

    auto comp = device().shader(StageCompute, "test/data/comp");
    const CpState::Config cconf{comp.get(), {}};
    auto cst = device().state(cconf);

    auto buf = device().buffer({1 << 14, Buffer::Shared, Buffer::CopySrc |
                                                         Buffer::CopyDst |
                                                         Buffer::Index |
                                                         Buffer::Vertex});

    Viewport vport{0.0f, 0.0f, 480.0f, 300.0f, 0.0f, 1.0f};
    Scissor sciss{{0, 0}, {480, 300}};
    TargetOp tgtOp{};

    GrEncoder enc1;
    enc1.setViewport(vport);
    enc1.setScissor(sciss);
    enc1.setTarget(*tgt, tgtOp);
    enc1.setState(*gst);
    enc1.setDcTable(1, 15);
    enc1.setVertexBuffer(*buf, 128, 0);
    enc1.setIndexBuffer(*buf, 256, IndexTypeU16);
    enc1.draw(0, 3, 0, 1);
    enc1.drawIndexed(6, 36, -6, 10, 50);

    CpEncoder enc2;
    enc2.setState(*cst);
    enc2.setDcTable(0, 0);
    enc2.setDcTable(1, 20);
    enc2.dispatch({64, 64, 16});

    TfEncoder enc3;
    enc3.copy(*buf, 3002, *buf, 60, 4096);
    enc3.copy(*img, {64, 32}, 4, 2, *img, {192, 16}, 1, 2, {64, 64}, 3);

    wstring str;
    bool chk;

    a.push_back({L"GrEncoder()", enc1.type() == Encoder::Graphics});
    for (auto& cmd : enc1.encoding()) {
      switch (cmd->cmd) {
      case Cmd::ViewportT: {
        auto sub = static_cast<ViewportCmd*>(cmd.get());
        str = L"Cmd::ViewportT";
        chk = sub->viewport == vport && sub->viewportIndex == 0;
      } break;
      case Cmd::ScissorT: {
        auto sub = static_cast<ScissorCmd*>(cmd.get());
        str = L"Cmd::ScissorT";
        chk = sub->scissor == sciss;
      } break;
      case Cmd::TargetT:
        str = L"Cmd::TargetT";
        chk = &static_cast<TargetCmd*>(cmd.get())->target == tgt.get();
        break;
      case Cmd::StateGrT:
        str = L"Cmd::StateGrT";
        chk = &static_cast<StateGrCmd*>(cmd.get())->state == gst.get();
        break;
      case Cmd::DcTableT: {
        auto sub = static_cast<DcTableCmd*>(cmd.get());
        str = L"Cmd::DcTableT";
        chk = sub->tableIndex == 1 && sub->allocIndex == 15;
      } break;
      case Cmd::VxBufferT: {
        auto sub = static_cast<VxBufferCmd*>(cmd.get());
        str = L"Cmd::VxBufferT";
        chk = &sub->buffer == buf.get() && sub->offset == 128 &&
              sub->inputIndex == 0;
      } break;
      case Cmd::IxBufferT: {
        auto sub = static_cast<IxBufferCmd*>(cmd.get());
        str = L"Cmd::IxBufferT";
        chk = &sub->buffer == buf.get() && sub->offset == 256 &&
              sub->type == IndexTypeU16;
      } break;
      case Cmd::DrawT: {
        auto sub = static_cast<DrawCmd*>(cmd.get());
        str = L"Cmd::DrawT";
        chk = sub->vertexStart == 0 && sub->vertexCount == 3 &&
              sub->baseInstance == 0 && sub->instanceCount == 1;
      } break;
      case Cmd::DrawIxT: {
        auto sub = static_cast<DrawIxCmd*>(cmd.get());
        str = L"Cmd::DrawIxT";
        chk = sub->indexStart == 6 && sub->vertexCount == 36 &&
              sub->vertexOffset == -6 && sub->baseInstance == 10 &&
              sub->instanceCount == 50;
      } break;
      default:
        str = L"#Invalid Cmd#";
        chk = false;
      }
      a.push_back({str, chk});
    }

    a.push_back({L"CpEncoder()", enc2.type() == Encoder::Compute});
    for (auto& cmd : enc2.encoding()) {
      switch (cmd->cmd) {
      case Cmd::StateCpT:
        str = L"Cmd::StateCpT";
        chk = &static_cast<StateCpCmd*>(cmd.get())->state == cst.get();
        break;
      case Cmd::DcTableT: {
        auto sub = static_cast<DcTableCmd*>(cmd.get());
        str = L"Cmd::DcTableT";
        chk = (sub->tableIndex == 0 && sub->allocIndex == 0) ||
              (sub->tableIndex == 1 && sub->allocIndex == 20);
      } break;
      case Cmd::DispatchT:
        str = L"Cmd::DispatchT";
        chk = static_cast<DispatchCmd*>(cmd.get())->size == Size3({64, 64, 16});
        break;
      default:
        str = L"#Invalid Cmd#";
        chk = false;
      }
      a.push_back({str, chk});
    }

    a.push_back({L"TfEncoder()", enc3.type() == Encoder::Transfer});
    for (auto& cmd : enc3.encoding()) {
      switch (cmd->cmd) {
      case Cmd::CopyBBT: {
        auto sub = static_cast<CopyBBCmd*>(cmd.get());
        str = L"Cmd::CopyBBT";
        chk = &sub->dst == &sub->src &&
              sub->dstOffset == 3002 && sub->srcOffset == 60 &&
              sub->size == 4096;
      } break;
      case Cmd::CopyIIT: {
        auto sub = static_cast<CopyIICmd*>(cmd.get());
        str = L"Cmd::CopyIIT";
        chk = &sub->dst == &sub->src &&
              sub->dstOffset == Offset2{64, 32} && sub->dstLayer == 4 &&
              sub->dstLevel == 2  &&
              sub->srcOffset == Offset2{192, 16} && sub->srcLayer == 1 &&
              sub->srcLevel == 2 &&
              sub->size == Size2{64, 64} && sub->layerCount == 3;
      } break;
      default:
        str = L"#Invalid Cmd#";
        chk = false;
      }
      a.push_back({str, chk});
    }

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* encoderTest() {
  static EncoderTest test;
  return &test;
}

TEST_NS_END
