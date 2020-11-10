//
// CG
// Device.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Device.h"
#include "vk/DeviceVK.h"

using namespace CG_NS;

Device::~Device() { }

// TODO
Device& Device::get() {
  return DeviceVK::get();
}
