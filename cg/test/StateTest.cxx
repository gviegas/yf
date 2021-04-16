//
// CG
// StateTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "State.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct StateTest : Test {
  StateTest() : Test(L"State") { }

  Assertions run(const vector<string>&) {
    struct GrState_ : GrState {
      GrState_(const Config& config) : GrState(config) { }
    };

    struct CpState_ : CpState {
      CpState_(const Config& config) : CpState(config) { }
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

    CpState::Config cc{nullptr, {}};
    CpState_ cs(cc);

    a.push_back({L"GrState(config)",
                 gs.config_.vxInputs.size() == 1 &&
                 gs.config_.vxInputs.back().attributes
                  .find(4)->second.format == VxFormatFlt4 &&
                 gs.config_.vxInputs.back().attributes
                  .find(4)->second.offset == 0 &&
                 gs.config_.primitive == PrimitiveTriangle &&
                 gs.config_.polyMode == PolyModeFill &&
                 gs.config_.cullMode == CullModeBack &&
                 gs.config_.winding == WindingCounterCw});

    a.push_back({L"CpState(config)",
                 cs.config_.shader == nullptr && cs.config_.dcTables.empty()});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* stateTest() {
  static StateTest test;
  return &test;
}

TEST_NS_END
