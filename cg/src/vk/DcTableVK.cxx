//
// CG
// DcTableVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "DcTableVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

DcTableVK::DcTableVK(const DcEntries& entries) : DcTable(entries) {
  // TODO
  throw runtime_error("Unimplemented");
}

DcTableVK::~DcTableVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyDescriptorPool(dev, pool_, nullptr);
  vkDestroyDescriptorSetLayout(dev, dsLayout_, nullptr);
}

void DcTableVK::allocate(uint32_t n) {
  // TODO
  throw runtime_error("Unimplemented");
}

uint32_t DcTableVK::allocations() const {
  return sets_.size();
}

void DcTableVK::write(uint32_t allocation,
                      DcId id,
                      uint32_t element,
                      Buffer& buffer,
                      uint64_t offset,
                      uint64_t size) {

  // TODO
  throw runtime_error("Unimplemented");
}

void DcTableVK::write(uint32_t allocation,
                      DcId id,
                      uint32_t element,
                      Image& image,
                      uint32_t layer) {

  // TODO
  throw runtime_error("Unimplemented");
}
