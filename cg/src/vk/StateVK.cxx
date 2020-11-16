//
// CG
// StateVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "StateVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

GrStateVK::GrStateVK(const Config& config) : GrState(config) {
  // TODO
  throw runtime_error("Unimplemented");
}

GrStateVK::~GrStateVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyPipeline(dev, pipeline_, nullptr);
  vkDestroyPipelineLayout(dev, plLayout_, nullptr);
}

VkPipeline GrStateVK::pipeline() const {
  return pipeline_;
}

CpStateVK::CpStateVK(const Config& config) : CpState(config) {
  // TODO
  throw runtime_error("Unimplemented");
}

CpStateVK::~CpStateVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyPipeline(dev, pipeline_, nullptr);
  vkDestroyPipelineLayout(dev, plLayout_, nullptr);
}

VkPipeline CpStateVK::pipeline() const {
  return pipeline_;
}
