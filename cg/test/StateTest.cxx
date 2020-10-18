//
// cg
// StateTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "yf/cg/State.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct StateTest : Test {
  StateTest() : Test(L"State") {}

  Assertions run(const vector<string>&) {
    struct GrState_ : GrState {
      GrState(const Config& config) : GrState(config) {}
    };

    struct CpState_ : CpState {
      CpState(const Config& config) : CpState(config) {}
    };

    Assertions a;

    GrState::Config gc;
    gc.vxInputs.push_back({});
    gc.vxInputs.back().attributes[4] = {VxFormatFlt4, 0};
    gc.vxInputs.back().stride = sizeof(float[4]);
    gc.vxInputs.back().stepFunction = VxStepFnVertex;
    gc.primitive = PrimitiveTriangle;
    gc.polyMode = PolyModeFill;
    gc.cullMode = CullModeBack;
    gc.winding = WindingCounterCw;
    GrState_ gs(gc);

    CpState::Config cc;
    CpState_ cs(cc);

    a.push_back({L"GrState(config)",
                 gs.config.vxInputs.size() == 1 &&
                 gs.config.vxInputs.back().attributes
                  .find(4)->second.format == VxFormatFlt4 &&
                 gs.config.vxInputs.back().attributes
                  .find(4)->second.offset == 0 &&
                 gs.config.primitive == PrimitiveTriangle &&
                 gs.config.polyMode == PolyModeFill &&
                 gs.config.cullMode == CullModeBack &&
                 gs.config.winding == WindingCounterCw});

    a.push_back({L"CpState(config)",
                 cs.config.shader == nullptr && cs.config.dcTables.empty()});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::stateTest() {
  static StateTest test;
  return &test;
}
