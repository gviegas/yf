//
// cg
// PassVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <stdexcept>

#include "PassVK.h"
#include "DeviceVK.h"

using namespace CG_NS;
using namespace std;

// ------------------------------------------------------------------------
// PassVK

PassVK::PassVK(const vector<ColorAttach>* colors,
               const vector<ColorAttach>* resolves,
               const DepStenAttach* depthStencil)
               : Pass(colors, resolves, depthStencil) {

  // TODO
}

PassVK::~PassVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyRenderPass(dev, _renderPass, nullptr);
}

Target::Ptr PassVK::makeTarget(Size2 size,
                               uint32_t layers,
                               const vector<AttachImg>* colors,
                               const vector<AttachImg>* resolves,
                               const AttachImg* depthStencil) {

  return make_unique<TargetVK>(*this, size, layers, colors, resolves,
                               depthStencil);
}

VkRenderPass PassVK::renderPass() const {
  return _renderPass;
}

// ------------------------------------------------------------------------
// TargetVK

TargetVK::TargetVK(PassVK& pass,
                   Size2 size,
                   uint32_t layers,
                   const vector<AttachImg>* colors,
                   const vector<AttachImg>* resolves,
                   const AttachImg* depthStencil)
                   : _pass(pass) {

  // TODO
}

TargetVK::~TargetVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyFramebuffer(dev, _framebuffer, nullptr);
}

const Pass& TargetVK::pass() const {
  return _pass;
}

VkFramebuffer TargetVK::framebuffer() const {
  return _framebuffer;
}
