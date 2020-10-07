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
    struct Device : CGDevice {
      unique_ptr<CGBuffer> makeBuffer(uint64_t) { return nullptr; }
      unique_ptr<CGImage> makeImage(CGPxFormat, CGSize2 size, uint32_t,
                                    uint32_t, CGSamples) { return nullptr; }
    };

    Device dev;

    // TODO
    vector<bool> res;
    res.push_back(dev.makeBuffer(1<<16) == nullptr);
    res.push_back(dev.makeBuffer(1<<27) != nullptr);
    res.push_back(
      dev.makeImage(CGPxFormatBgra8Srgb, 1024, 1, 1, CGSamples1) == nullptr);

    double cov = count(res.begin(), res.end(), true);
    return cov / res.size();
  }
};

INTERNAL_NS_END

Test* TEST_NS::deviceTest() {
  static DeviceTest test;
  return &test;
}
