//
// CG
// WsiVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "WsiVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

WsiVK::WsiVK(WS_NS::Window* window) : Wsi(window) {
  if (!window_)
    throw invalid_argument("WsiVK requires a valid window object");

  // TODO
  throw runtime_error("Unimplemented");
}

WsiVK::~WsiVK() {
  // TODO
}

const vector<Image*>& WsiVK::images() const {
  return images_;
}

Image* WsiVK::nextImage() {
  // TODO
  throw runtime_error("Unimplemented");
}

void WsiVK::present() {
  // TODO
  throw runtime_error("Unimplemented");
}
