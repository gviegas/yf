//
// CG
// DeviceTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "yf/cg/Device.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct DeviceTest : Test {
  DeviceTest() : Test(L"Device") {}

  Assertions run(const vector<string>&) {
    Assertions a;

    auto& dev = Device::get();

    auto shd = dev.makeShader(StageFragment, L"tmp/frag");

    vector<ColorAttach> clrs{{PxFormatRgba8Unorm, Samples1, LoadOpLoad,
                              StoreOpStore}};
    DepStenAttach depSten = {PxFormatD16Unorm, Samples1, LoadOpLoad,
                             StoreOpStore, LoadOpDontCare, StoreOpDontCare};
    auto pass = dev.makePass(&clrs, nullptr, &depSten);

    auto buf = dev.makeBuffer(1<<28);

    a.push_back({L"Device::get()", true});
    a.push_back({L"dev.makeShader(...)", shd != nullptr});
    a.push_back({L"dev.makePass(...)", pass != nullptr});
    a.push_back({L"dev.makeBuffer(...)", buf != nullptr});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::deviceTest() {
  static DeviceTest test;
  return &test;
}
