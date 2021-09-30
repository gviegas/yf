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
    class GrState_ : public GrState {
      Config config_;
     public:
      GrState_(const Config& config) : config_(config) { }
      const Config& config() const { return config_; }
    };

    class CpState_ : public CpState {
      Config config_;
     public:
      CpState_(const Config& config) : config_(config) { }
      const Config& config() const { return config_; }
    };

    Assertions a;

    GrState::Config gc;
    gc.vxInputs.push_back({});
    gc.vxInputs.back().attributes.push_back({3, VxFormatFlt4, 0});
    gc.vxInputs.back().stride = sizeof(float[4]);
    gc.vxInputs.back().stepFunction = VxStepFnVertex;
    gc.topology = TopologyTriangle;
    gc.polyMode = PolyModeFill;
    gc.cullMode = CullModeBack;
    gc.winding = WindingCounterCw;
    GrState_ gs(gc);

    CpState::Config cc{nullptr, {}};
    CpState_ cs(cc);

    a.push_back({L"GrState(config)",
                 gs.config().vxInputs.size() == 1 &&
                 gs.config().vxInputs.back().attributes.size() == 1 &&
                 gs.config().vxInputs.back().attributes.back().id == 3 &&
                 gs.config().vxInputs.back().attributes.back().format
                  == VxFormatFlt4 &&
                 gs.config().vxInputs.back().attributes.back().offset == 0 &&
                 gs.config().topology == TopologyTriangle &&
                 gs.config().polyMode == PolyModeFill &&
                 gs.config().cullMode == CullModeBack &&
                 gs.config().winding == WindingCounterCw});

    a.push_back({L"CpState(config)",
                 cs.config().shader == nullptr &&
                 cs.config().dcTables.empty()});

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
