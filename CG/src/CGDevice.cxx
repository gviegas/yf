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
    BufferRes makeBuffer(uint64_t) { return BufferRes(nullptr); }

    ImageRes makeImage(CGPxFormat, CGSize2, uint32_t, uint32_t, CGSamples)
    { return ImageRes(nullptr); }

    ShaderRes makeShader(CGStage, wstring&&, wstring&&)
    { return ShaderRes(nullptr); }

    DcTableRes makeDcTable(const CGDcEntries&)
    { return DcTableRes(nullptr); }
    DcTableRes makeDcTable(CGDcEntries&&)
    { return DcTableRes(nullptr); }

    PassRes makePass(const std::vector<CGColorAttach>*,
                     const std::vector<CGColorAttach>*,
                     const CGDepStenAttach*)
    { return PassRes(nullptr); }

    GrStateRes makeState(const CGGrState::Config&)
    { return GrStateRes(nullptr); }
    GrStateRes makeState(CGGrState::Config&&)
    { return GrStateRes(nullptr); }
    CpStateRes makeState(const CGCpState::Config&)
    { return CpStateRes(nullptr); }
    CpStateRes makeState(CGCpState::Config&&)
    { return CpStateRes(nullptr); }

    QueueRes defaultQueue()
    { return QueueRes(nullptr); }
    QueueRes queue(CGQueue::CapabilityMask)
    { return QueueRes(nullptr); }
  };

  static Dummy dev;
  return dev;
  ////////////////

};
