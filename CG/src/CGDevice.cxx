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
    unique_ptr<CGBuffer> makeBuffer(uint64_t) { return nullptr; }
    unique_ptr<CGImage> makeImage(CGPxFormat, CGSize2, uint32_t, uint32_t,
                                  CGSamples) { return nullptr; }
  };
  static Dummy dev;
  return dev;
  ////////////////

};
