//
// yf
// DeviceVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

//??
#include <iostream>
//??

#include <cassert>

#include "DeviceVK.h"
#include "VK.h"

using namespace YF_NS;
using namespace std;

// TODO
class DeviceVK::Impl {
 public:
  Impl() {
    if (!initVK()) {
      // TODO
      throw std::runtime_error("!initVK()");
    }

    //??
    uint32_t version;
    if (!vk._1.enumerateInstanceVersion)
      version = VK_API_VERSION_1_0;
    else
      vk._1.enumerateInstanceVersion(&version);

    wcout << "\nVersion is "
          << VK_VERSION_MAJOR(version) << "."
          << VK_VERSION_MINOR(version) << endl;

    exit(0);
    //??
  }
};

DeviceVK::DeviceVK() : _impl(make_unique<Impl>()) {}

DeviceVK::~DeviceVK() {}

CGDevice::BufferRes DeviceVK::makeBuffer(uint64_t size) {
  // TODO
  assert(false);
}

CGDevice::ImageRes DeviceVK::makeImage(CGPxFormat format,
                                       CGSize2 size,
                                       uint32_t layers,
                                       uint32_t levels,
                                       CGSamples samples) {
  // TODO
  assert(false);
}

CGDevice::ShaderRes DeviceVK::makeShader(CGStage stage,
                                         wstring&& codeFile,
                                         wstring&& entryPoint) {
  // TODO
  assert(false);
}

CGDevice::DcTableRes DeviceVK::makeDcTable(const CGDcEntries& entries) {
  // TODO
  assert(false);
}

CGDevice::DcTableRes DeviceVK::makeDcTable(CGDcEntries&& entries) {
  // TODO
  assert(false);
}

CGDevice::PassRes DeviceVK::makePass(const vector<CGColorAttach>* colors,
                                     const vector<CGColorAttach>* resolves,
                                     const CGDepStenAttach* depthStencil) {
  // TODO
  assert(false);
}

CGDevice::GrStateRes DeviceVK::makeState(const CGGrState::Config& config) {
  // TODO
  assert(false);
}

CGDevice::GrStateRes DeviceVK::makeState(CGGrState::Config&& config) {
  // TODO
  assert(false);
}

CGDevice::CpStateRes DeviceVK::makeState(const CGCpState::Config& config) {
  // TODO
  assert(false);
}

CGDevice::CpStateRes DeviceVK::makeState(CGCpState::Config&& config) {
  // TODO
  assert(false);
}

CGDevice::QueueRes DeviceVK::defaultQueue() {
  // TODO
  assert(false);
}

CGDevice::QueueRes DeviceVK::queue(CGQueue::CapabilityMask capabilities) {
  // TODO
  assert(false);
}
