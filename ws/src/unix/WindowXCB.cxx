//
// WS
// WindowXCB.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "WindowXCB.h"

using namespace WS_NS;

WindowXCB::WindowXCB(uint32_t width, uint32_t height, CreationMask mask)
  : width_(width), height_(height), mask_(mask) {

  // TODO
}

WindowXCB::~WindowXCB() {
  // TODO
}

void WindowXCB::close() {
  // TODO
}

void WindowXCB::toggleFullscreen() {
  // TODO
}

void WindowXCB::resize(uint32_t width, uint32_t height) {
  // TODO
}

void WindowXCB::show() {
  // TODO
}

void WindowXCB::hide() {
  // TODO
}

uint32_t WindowXCB::width() const {
  return width_;
}

uint32_t WindowXCB::height() const {
  return height_;
}
