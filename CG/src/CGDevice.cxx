//
// yf
// CGDevice.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include "CGDevice.h"

using namespace YF_NS;
using namespace std;

CGDevice::~CGDevice() {}

// TODO
CGDevice& CGDevice::get() {

  ////////////////
  struct Dummy : CGDevice {
    BufferPtr makeBuffer(uint64_t) { return nullptr; }

    ImagePtr makeImage(CGPxFormat, CGSize2, uint32_t, uint32_t, CGSamples)
    { return nullptr; }

    ShaderPtr makeShader(CGStage, wstring&&, wstring&&) { return nullptr; }

    DcTablePtr makeDcTable(const CGDcEntries&) { return nullptr; }
    DcTablePtr makeDcTable(CGDcEntries&&) { return nullptr; }
  };

  static Dummy dev;
  return dev;
  ////////////////

};
