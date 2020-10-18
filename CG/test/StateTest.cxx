//
// yf
// StateTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
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

  Assertions run(const vector<string>&) {
    struct GrState : CGGrState {
      GrState(const Config& config) : CGGrState(config) {}
    };

    struct CpState : CGCpState {
      CpState(const Config& config) : CGCpState(config) {}
    };

    Assertions a;

    GrState::Config gc;
    gc.vxInputs.push_back({});
    gc.vxInputs.back().attributes[4] = {CGVxFormatFlt4, 0};
    gc.vxInputs.back().stride = sizeof(float[4]);
    gc.vxInputs.back().stepFunction = CGVxStepFnVertex;
    gc.primitive = CGPrimitiveTriangle;
    gc.polyMode = CGPolyModeFill;
    gc.cullMode = CGCullModeBack;
    gc.winding = CGWindingCounterCw;
    GrState gs(gc);

    CpState::Config cc;
    CpState cs(cc);

    a.push_back({L"CGGrState(config)",
                 gs.config.vxInputs.size() == 1 &&
                 gs.config.vxInputs.back().attributes
                  .find(4)->second.format == CGVxFormatFlt4 &&
                 gs.config.vxInputs.back().attributes
                  .find(4)->second.offset == 0 &&
                 gs.config.primitive == CGPrimitiveTriangle &&
                 gs.config.polyMode == CGPolyModeFill &&
                 gs.config.cullMode == CGCullModeBack &&
                 gs.config.winding == CGWindingCounterCw});

    a.push_back({L"CGCpState(config)",
                 cs.config.shader == nullptr && cs.config.dcTables.empty()});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::stateTest() {
  static StateTest test;
  return &test;
}
