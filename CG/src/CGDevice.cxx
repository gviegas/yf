//
// yf
// CGDevice.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
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

    PassPtr makePass(const std::vector<CGColorAttach>*,
                     const std::vector<CGColorAttach>*,
                     const CGDepStenAttach*) { return nullptr; }

    GrStatePtr makeState(const CGGrState::Config&) { return nullptr; }
    GrStatePtr makeState(CGGrState::Config&&) { return nullptr; }
    CpStatePtr makeState(const CGCpState::Config&) { return nullptr; }
    CpStatePtr makeState(CGCpState::Config&&){ return nullptr; }

    CGQueue& defaultQueue() { std::abort(); }
    CGQueue* queue(CGQueue::CapabilityMask) { return nullptr; }
  };

  static Dummy dev;
  return dev;
  ////////////////

};
