//
// yf
// EncoderTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "CGEncoder.h"
#include "CGCmd.h"
#include "Defs.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct EncoderTest : Test {
  EncoderTest() : Test(L"CGEncoder") {}

  Assertions run(const vector<string>& args) {
    Assertions a;

    CGViewport vport{0.0f, 0.0f, 480.0f, 300.0f, 0.0f, 1.0f};
    CGScissor sciss{{0, 0}, {480, 300}};
    CGColor green{0.0f, 1.0f, 0.0f, 1.0f};

    CGEncoder enc;
    enc.setState(static_cast<CGGrState*>(nullptr));
    enc.setState(static_cast<CGCpState*>(nullptr));
    enc.setViewport(vport);
    enc.setScissor(sciss);
    enc.setTarget(nullptr);
    enc.setDcTable(1, 15);
    enc.setVertexBuffer(nullptr, 128);
    enc.setIndexBuffer(nullptr, 256, CGIndexTypeU16);
    enc.draw(0, 3, 0, 1);
    enc.drawIndexed(6, 36, -6, 10, 50);
    enc.dispatch(64);
    enc.clearColor(green);
    enc.clearDepth(1.0f);
    enc.clearStencil(0xFF);

    wstring str;
    bool chk;
    for (auto& cmd : enc.encoding()) {
      switch (cmd->cmd) {
      case CGCmd::StateGr:
        str = L"CGCmd::StateGr";
        chk = static_cast<CGStateGrCmd*>(cmd.get())->state == nullptr;
        break;
      case CGCmd::StateCp:
        str = L"CGCmd::StateCp";
        chk = static_cast<CGStateCpCmd*>(cmd.get())->state == nullptr;
        break;
      case CGCmd::Viewport: {
        auto sub = static_cast<CGViewportCmd*>(cmd.get());
        str = L"CGCmd::Viewport";
        chk = sub->viewport == vport && sub->viewportIndex == 0;
      } break;
      case CGCmd::Scissor: {
        auto sub = static_cast<CGScissorCmd*>(cmd.get());
        str = L"CGCmd::Scissor";
        chk = sub->scissor == sciss;
      } break;
      case CGCmd::Target:
        str = L"CGCmd::Target";
        chk = static_cast<CGTargetCmd*>(cmd.get())->target == nullptr;
        break;
      case CGCmd::DcTable: {
        auto sub = static_cast<CGDcTableCmd*>(cmd.get());
        str = L"CGCmd::DcTable";
        chk = sub->tableIndex == 1 && sub->allocIndex == 15;
      } break;
      case CGCmd::VxBuffer: {
        auto sub = static_cast<CGVxBufferCmd*>(cmd.get());
        str = L"CGCmd::VxBuffer";
        chk = sub->buffer == nullptr && sub->offset == 128 &&
              sub->inputIndex == 0;
      } break;
      case CGCmd::IxBuffer: {
        auto sub = static_cast<CGIxBufferCmd*>(cmd.get());
        str = L"CGCmd::IxBuffer";
        chk = sub->buffer == nullptr && sub->offset == 256 &&
              sub->type == CGIndexTypeU16;
      } break;
      case CGCmd::Draw: {
        auto sub = static_cast<CGDrawCmd*>(cmd.get());
        str = L"CGCmd::Draw";
        chk = sub->vertexStart == 0 && sub->vertexCount == 3 &&
              sub->baseInstance == 0 && sub->instanceCount == 1;
      } break;
      case CGCmd::DrawIx: {
        auto sub = static_cast<CGDrawIxCmd*>(cmd.get());
        str = L"CGCmd::DrawIx";
        chk = sub->indexStart == 6 && sub->vertexCount == 36 &&
              sub->vertexOffset == -6 && sub->baseInstance == 10 &&
              sub->instanceCount == 50;
      } break;
      case CGCmd::Dispatch:
        str = L"CGCmd::Dispatch";
        chk = static_cast<CGDispatchCmd*>(cmd.get())->size == CGSize3(64);
        break;
      case CGCmd::ClearCl: {
        auto sub = static_cast<CGClearClCmd*>(cmd.get());
        str = L"CGCmd::ClearCl";
        chk = sub->value == green && sub->colorIndex == 0;
      } break;
      case CGCmd::ClearDp:
        str = L"CGCmd::ClearDp";
        chk = static_cast<CGClearDpCmd*>(cmd.get())->value == 1.0f;
        break;
      case CGCmd::ClearSc:
        str = L"CGCmd::ClearSc";
        chk = static_cast<CGClearScCmd*>(cmd.get())->value == 0xFF;
        break;
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
