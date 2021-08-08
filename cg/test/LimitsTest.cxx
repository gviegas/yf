//
// CG
// LimitsTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "Test.h"
#include "Limits.h"
#include "Device.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct LimitsTest : Test {
  LimitsTest() : Test(L"Limits") { }

  Assertions run(const vector<string>&) {
    const auto& lim = device().limits();

    wcout << "\nLimits:\n"
          << "\n maxDrawIndex      = " << lim.maxDrawIndex
          << "\n maxDispatchWidth  = " << lim.maxDispatchWidth
          << "\n maxDispatchheight = " << lim.maxDispatchHeight
          << "\n maxDispatchDepth  = " << lim.maxDispatchDepth
          << "\n"
          << "\n maxViewports      = " << lim.maxViewports
          << "\n maxViewportWidth  = " << lim.maxViewportWidth
          << "\n maxViewportHeight = " << lim.maxViewportHeight
          << "\n"
          << "\n maxImageWidth  = " << lim.maxImageWidth
          << "\n maxImageHeight = " << lim.maxImageHeight
          << "\n maxImageLayers = " << lim.maxImageLayers
          << "\n"
          << "\n maxPassColors   = " << lim.maxPassColors
          << "\n maxTargetWidth  = " << lim.maxTargetWidth
          << "\n maxTargetHeight = " << lim.maxTargetHeight
          << "\n maxTargetLayers = " << lim.maxTargetLayers
          << "\n"
          << "\n maxDcUniform    = " << lim.maxDcUniform
          << "\n maxDcStorage    = " << lim.maxDcStorage
          << "\n maxDcImage      = " << lim.maxDcImage
          << "\n maxDcImgSampler = " << lim.maxDcImgSampler
          << "\n maxDcEntries    = " << lim.maxDcEntries
          << "\n minDcUniformWriteAlignedOffset = "
          << lim.minDcUniformWriteAlignedOffset
          << "\n maxDcUniformWriteSize          = "
          << lim.maxDcUniformWriteSize
          << "\n minDcStorageWriteAlignedOffset = "
          << lim.minDcStorageWriteAlignedOffset
          << "\n maxDcStorageWriteSize          = "
          << lim.maxDcStorageWriteSize
          << "\n"
          << "\n maxVxInputs = " << lim.maxVxInputs
          << "\n maxVxAttrs  = " << lim.maxVxAttrs
          << "\n";

    return {{L"device().limits()", true}};
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* limitsTest() {
  static LimitsTest test;
  return &test;
}

TEST_NS_END
