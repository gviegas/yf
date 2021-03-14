//
// CG
// DeviceTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
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
    auto& que = dev.defaultQueue();
    auto win = WS_NS::createWindow(400, 400, L"Device Test",
                                   WS_NS::Window::Resizable);
    auto wsi = dev.wsi(win.get());

    this_thread::sleep_for(chrono::seconds(1));

    a.push_back({L"Device::get()", true});
    a.push_back({L"dev.defaultQueue()", que.capabilities_ != 0});
    a.push_back({L"dev.makeWsi(...)", wsi != nullptr});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::deviceTest() {
  static DeviceTest test;
  return &test;
}
