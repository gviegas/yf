//
// CG
// DeviceTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <thread>

#include "UnitTests.h"
#include "Device.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct DeviceTest : Test {
  DeviceTest() : Test(L"Device") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    auto& dev = Device::get();
    auto vert = dev.makeShader(StageVertex, L"tmp/vert");
    auto frag = dev.makeShader(StageFragment, L"tmp/frag");
    vector<ColorAttach> clrs{
      {PxFormatRgba8Unorm, Samples1, LoadOpLoad, StoreOpStore}};
    auto pass = dev.makePass(&clrs, nullptr, nullptr);
    GrState::Config config{
      pass.get(),
      {vert.get(), frag.get()},
      {},
      {{{{0, {VxFormatFlt3, 0}}}, 24, VxStepFnVertex}},
      PrimitiveTriangle,
      PolyModeFill,
      CullModeBack,
      WindingCounterCw
    };
    auto state = dev.makeState(config);
    auto win = WS_NS::Window::make(400, 400, L"Device Test",
                                   WS_NS::Window::Resizable);
    auto wsi = dev.makeWsi(win.get());

    this_thread::sleep_for(chrono::seconds(1));

    a.push_back({L"Device::get()", true});
    a.push_back({L"dev.makeShader(...)", vert != nullptr && frag != nullptr});
    a.push_back({L"dev.makePass(...)", pass != nullptr});
    a.push_back({L"dev.makeState(...)", state != nullptr});
    a.push_back({L"dev.makeWsi(...)", wsi != nullptr});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::deviceTest() {
  static DeviceTest test;
  return &test;
}
