//
// CG
// Device.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Device.h"
#include "vk/DeviceVK.h"

using namespace CG_NS;

// TODO
Device& CG_NS::device() {
  return deviceVK();
}

Device::~Device() { }
