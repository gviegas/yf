//
// yf
// DeviceTest.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include <algorithm>
#include <iostream>

#include "Defs.h"
#include "UnitTests.h"
#include "CGDevice.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct DeviceTest : Test {
  DeviceTest() : Test(L"CGDevice") {}

  Coverage run(const vector<string>& args) {
    auto& dev = CGDevice::get();

    // TODO
    return 1.0;
  }
};

INTERNAL_NS_END

Test* TEST_NS::deviceTest() {
  static DeviceTest test;
  return &test;
}
