//
// CG
// ImageVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cstring>

#include "ImageVK.h"
#include "MemoryVK.h"
#include "QueueVK.h"
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
    layout_ = nextLayout_ = VK_IMAGE_LAYOUT_PREINITIALIZED;
  } else {
    usage_ = fmtProp.optimalTilingFeatures;
    if (usage_ == 0)
      throw UnsupportedExcept("Format not supported by ImageVK");
    tiling_ = VK_IMAGE_TILING_OPTIMAL;
    layout_ = nextLayout_ = VK_IMAGE_LAYOUT_UNDEFINED;
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

  if (offset.x + size.width > size_.width ||
      offset.y + size.height > size_.height ||
      layer >= layers_ ||
      level >= levels_ ||
      !data)
    throw invalid_argument("ImageVK write()");

  if (layout_ != VK_IMAGE_LAYOUT_PREINITIALIZED &&
      layout_ != VK_IMAGE_LAYOUT_GENERAL)
    changeLayout(VK_IMAGE_LAYOUT_GENERAL, false);

  if (tiling_ == VK_IMAGE_TILING_LINEAR) {
    VkImageSubresource subres;
    // TODO: ensure that a single aspect is set (no combined depth/stencil)
    subres.aspectMask = aspectOfVK(format_);
    subres.mipLevel = level;
    subres.arrayLayer = 0;//layer;

    auto dev = DeviceVK::get().device();
    VkSubresourceLayout layout;
    // TODO: consider getting all required layouts once on creation
    vkGetImageSubresourceLayout(dev, handle_, &subres, &layout);

    auto dst = reinterpret_cast<uint8_t*>(data_);
    dst += layout.offset + layout.arrayPitch*layer;
    auto src = reinterpret_cast<const uint8_t*>(data);
    for (uint32_t row = 0; row < size.height; ++row) {
      memcpy(dst, src, size.width);
      dst += layout.rowPitch;
      src += size.width;
    }

  } else {
    // TODO
    throw runtime_error("Unimplemented");
  }
}

void ImageVK::changeLayout(VkImageLayout newLayout, bool defer) {
  if (nextLayout_ == newLayout)
    return;

  if (layout_ != nextLayout_)
    throw runtime_error("Multiple layout transitions requested");

  nextLayout_ = newLayout;

  VkImageMemoryBarrier imb;
  imb.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  imb.pNext = nullptr;
  imb.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
  imb.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  imb.oldLayout = layout_;
  imb.newLayout = newLayout;
  imb.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imb.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imb.image = handle_;
  imb.subresourceRange.aspectMask = aspectOfVK(format_);
  imb.subresourceRange.baseMipLevel = 0;
  imb.subresourceRange.levelCount = levels_;
  imb.subresourceRange.baseArrayLayer = 0;
  imb.subresourceRange.layerCount = layers_;

  auto& queue = static_cast<QueueVK&>(DeviceVK::get().defaultQueue());
  auto cb = queue.getPriority([&](bool result) {
    if (result)
      layout_ = nextLayout_;
    else
      nextLayout_ = layout_;
  });

  vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,
                       0, nullptr, 0, nullptr, 1, &imb);
  if (!defer)
    queue.submit();
}

VkImage ImageVK::handle() const {
  return handle_;
}
