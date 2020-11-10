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
using namespace std;

ImageVK::ImageVK(PxFormat format,
                 Size2 size,
                 uint32_t layers,
                 uint32_t levels,
                 Samples samples)
                 : Image(format, size, layers, levels, samples) {

  if (size == 0)
    throw invalid_argument("ImageVK requires size != 0");
  if (layers == 0)
    throw invalid_argument("ImageVK requires layers != 0");
  if (levels == 0)
    throw invalid_argument("ImageVK requires levels != 0");

  VkFormat fmt = toFormatVK(format);
  if (fmt == VK_FORMAT_UNDEFINED)
    throw invalid_argument("ImageVK requires a valid format");

  VkSampleCountFlagBits smpl = toSampleCountVK(samples);

  if (size.height > 1)
    type_ = VK_IMAGE_TYPE_2D;
  else
    type_ = VK_IMAGE_TYPE_1D;

  auto phys = DeviceVK::get().physicalDev();
  VkFormatProperties fmtProp;
  vkGetPhysicalDeviceFormatProperties(phys, fmt, &fmtProp);

  auto getUsage = [](VkFormatFeatureFlags feat) {
    VkImageUsageFlags usage = 0;

    if (feat & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)
      usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if (feat & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)
      usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    if (feat & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)
      usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (feat & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
      usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if (DeviceVK::get().devVersion() >= VK_API_VERSION_1_1) {
      if (feat & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT)
        usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      if (feat & VK_FORMAT_FEATURE_TRANSFER_DST_BIT)
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    } else {
      // XXX: not in v1.0
      usage |= VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
               VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    }

    return usage;
  };

  // TODO: improve this
  usage_ = getUsage(fmtProp.linearTilingFeatures);
  if (usage_ != 0) {
    tiling_ = VK_IMAGE_TILING_LINEAR;
    layout_ = VK_IMAGE_LAYOUT_PREINITIALIZED;
  } else {
    usage_ = fmtProp.optimalTilingFeatures;
    if (usage_ == 0)
      throw UnsupportedExcept("Format not supported by ImageVK");
    tiling_ = VK_IMAGE_TILING_OPTIMAL;
    layout_ = VK_IMAGE_LAYOUT_UNDEFINED;
  }

  auto dev = DeviceVK::get().device();
  VkResult res;

  VkImageCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.imageType = type_;
  info.format = fmt;
  info.extent = {size.width, size.height, 1};
  info.mipLevels = levels;
  info.arrayLayers = layers;
  info.samples = smpl;
  info.tiling = tiling_;
  info.usage = usage_;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.queueFamilyIndexCount = 0;
  info.pQueueFamilyIndices = nullptr;
  info.initialLayout = layout_;

  res = vkCreateImage(dev, &info, nullptr, &handle_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create image");

  VkMemoryRequirements memReq;
  vkGetImageMemoryRequirements(dev, handle_, &memReq);

  try {
    memory_ = allocateVK(memReq, tiling_ == VK_IMAGE_TILING_OPTIMAL);
  } catch (...) {
    vkDestroyImage(dev, handle_, nullptr);
    throw;
  }

  res = vkBindImageMemory(dev, handle_, memory_, 0);
  if (res != VK_SUCCESS) {
    vkDestroyImage(dev, handle_, nullptr);
    deallocateVK(memory_);
    throw DeviceExcept("Failed to bind memory to image");
  }

  if (tiling_ == VK_IMAGE_TILING_LINEAR) {
    res = vkMapMemory(dev, memory_, 0, VK_WHOLE_SIZE, 0, &data_);
    if (res != VK_SUCCESS) {
      vkDestroyImage(dev, handle_, nullptr);
      deallocateVK(memory_);
      throw DeviceExcept("Failed to map image memory");
    }
  }
}

ImageVK::~ImageVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyImage(dev, handle_, nullptr);
  deallocateVK(memory_);
}

void ImageVK::write(Offset2 offset,
                    Size2 size,
                    uint32_t layer,
                    uint32_t level,
                    const void* data) {

  // TODO
  throw runtime_error("Unimplemented");
}

VkImage ImageVK::handle() const {
  return handle_;
}
