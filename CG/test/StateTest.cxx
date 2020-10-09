//
// yf
// StateTest.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "CGState.h"
#include "Defs.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct StateTest : Test {
  StateTest() : Test(L"CGState") {}

  Assertions run(const vector<string>& args) {
    struct GraphState : CGGraphState {
      GraphState(const Config& config) : CGGraphState(config) {}
    };

    struct CompState : CGCompState {
      CompState(const Config& config) : CGCompState(config) {}
    };

    Assertions a;

    GraphState::Config gc;
    gc.vxInputs.push_back({});
    gc.vxInputs.back().attributes[4] = {CGVxFormatFlt4, 0};
    gc.vxInputs.back().stride = sizeof(float[4]);
    gc.vxInputs.back().stepFunction = CGVxStepFnVertex;
    gc.primitive = CGPrimitiveTriangle;
    gc.polyMode = CGPolyModeFill;
    gc.cullMode = CGCullModeBack;
    gc.winding = CGWindingCounterCw;
    GraphState gs(gc);

    CompState::Config cc;
    CompState cs(cc);

    a.push_back({L"CGGraphState(config)",
                 gs.config.vxInputs.size() == 1 &&
                 gs.config.vxInputs.back().attributes
                  .find(4)->second.format == CGVxFormatFlt4 &&
                 gs.config.vxInputs.back().attributes
                  .find(4)->second.offset == 0 &&
                 gs.config.primitive == CGPrimitiveTriangle &&
                 gs.config.polyMode == CGPolyModeFill &&
                 gs.config.cullMode == CGCullModeBack &&
                 gs.config.winding == CGWindingCounterCw});

    a.push_back({L"CGCompState(config)",
                 cs.config.shader == nullptr && cs.config.dcTables.empty()});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::stateTest() {
  static StateTest test;
  return &test;
}
