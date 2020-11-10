//
// CG
// EncoderTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "yf/cg/Encoder.h"
#include "Cmd.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct EncoderTest : Test {
  EncoderTest() : Test(L"Encoder") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    Viewport vport{0.0f, 0.0f, 480.0f, 300.0f, 0.0f, 1.0f};
    Scissor sciss{{0, 0}, {480, 300}};
    Color green{0.0f, 1.0f, 0.0f, 1.0f};

    GrEncoder enc1;
    enc1.setState(nullptr);
    enc1.setViewport(vport);
    enc1.setScissor(sciss);
    enc1.setTarget(nullptr);
    enc1.setDcTable(1, 15);
    enc1.setVertexBuffer(nullptr, 128);
    enc1.setIndexBuffer(nullptr, 256, IndexTypeU16);
    enc1.draw(0, 3, 0, 1);
    enc1.drawIndexed(6, 36, -6, 10, 50);
    enc1.clearColor(green);
    enc1.clearDepth(1.0f);
    enc1.clearStencil(0xFF);

    CpEncoder enc2;
    enc2.setDcTable(0, 0);
    enc2.setDcTable(1, 20);
    enc2.dispatch(64);

    wstring str;
    bool chk;

    a.push_back({L"GrEncoder()", enc1.type() == Encoder::Graphics});
    for (auto& cmd : enc1.encoding()) {
      switch (cmd->cmd) {
      case Cmd::StateGrT:
        str = L"Cmd::StateGrT";
        chk = static_cast<StateGrCmd*>(cmd.get())->state == nullptr;
        break;
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
        chk = static_cast<TargetCmd*>(cmd.get())->target == nullptr;
        break;
      case Cmd::DcTableT: {
        auto sub = static_cast<DcTableCmd*>(cmd.get());
        str = L"Cmd::DcTableT";
        chk = sub->tableIndex == 1 && sub->allocIndex == 15;
        } break;
      case Cmd::VxBufferT: {
        auto sub = static_cast<VxBufferCmd*>(cmd.get());
        str = L"Cmd::VxBufferT";
        chk = sub->buffer == nullptr && sub->offset == 128 &&
              sub->inputIndex == 0;
        } break;
      case Cmd::IxBufferT: {
        auto sub = static_cast<IxBufferCmd*>(cmd.get());
        str = L"Cmd::IxBufferT";
        chk = sub->buffer == nullptr && sub->offset == 256 &&
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
      case Cmd::ClearClT: {
        auto sub = static_cast<ClearClCmd*>(cmd.get());
        str = L"Cmd::ClearClT";
        chk = sub->value == green && sub->colorIndex == 0;
        } break;
      case Cmd::ClearDpT:
        str = L"Cmd::ClearDpT";
        chk = static_cast<ClearDpCmd*>(cmd.get())->value == 1.0f;
        break;
      case Cmd::ClearScT:
        str = L"Cmd::ClearScT";
        chk = static_cast<ClearScCmd*>(cmd.get())->value == 0xFF;
        break;
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
        chk = static_cast<StateCpCmd*>(cmd.get())->state == nullptr;
        break;
      case Cmd::DcTableT: {
        auto sub = static_cast<DcTableCmd*>(cmd.get());
        str = L"Cmd::DcTableT";
        chk = (sub->tableIndex == 0 && sub->allocIndex == 0) ||
              (sub->tableIndex == 1 && sub->allocIndex == 20);
        } break;
      case Cmd::DispatchT:
        str = L"Cmd::DispatchT";
        chk = static_cast<DispatchCmd*>(cmd.get())->size == Size3(64);
        break;
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

Test* TEST_NS::encoderTest() {
  static EncoderTest test;
  return &test;
}
