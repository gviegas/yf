//
// yf
// DeviceTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "CGDevice.h"
#include "Defs.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct DeviceTest : Test {
  DeviceTest() : Test(L"CGDevice") {}

  Assertions run(const vector<string>&) {
    auto& dev = CGDevice::get();

    // TODO
    return {};
  }
};

INTERNAL_NS_END

Test* TEST_NS::deviceTest() {
  static DeviceTest test;
  return &test;
}
