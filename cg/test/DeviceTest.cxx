//
// CG
// DeviceTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <thread>

#include "Test.h"
#include "Device.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct DeviceTest : Test {
  DeviceTest() : Test(L"Device") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    auto& dev = device();
    auto& que = dev.defaultQueue();
    auto win = WS_NS::createWindow(400, 400, name_, WS_NS::Window::Resizable);
    auto wsi = dev.wsi(*win);

    this_thread::sleep_for(chrono::seconds(1));

    a.push_back({L"device()", true});
    a.push_back({L"dev.defaultQueue()", que.capabilities() != 0});
    a.push_back({L"dev.wsi(...)", wsi != nullptr});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* deviceTest() {
  static DeviceTest test;
  return &test;
}

TEST_NS_END
