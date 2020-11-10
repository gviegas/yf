//
// CG
// ImageVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cstring>

#include "ImageVK.h"
#include "MemoryVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;

ImageVK::ImageVK(PxFormat format,
                 Size2 size,
                 uint32_t layers,
                 uint32_t levels,
                 Samples samples)
                 : Image(format, size, layers, levels, samples) {

  // TODO
}

ImageVK::~ImageVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyImage(dev, handle_, nullptr);
}

void ImageVK::write(Offset2 offset,
                    Size2 size,
                    uint32_t layer,
                    uint32_t level,
                    const void* data) {

  // TODO
}

VkImage ImageVK::handle() const {
  return handle_;
}
