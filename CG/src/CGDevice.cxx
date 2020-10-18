//
// yf
// CGDevice.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "CGDevice.h"
#include "vk/DeviceVK.h"

using namespace YF_NS;

CGDevice::~CGDevice() {}

// TODO
CGDevice& CGDevice::get() {
  return DeviceVK::get();
}
