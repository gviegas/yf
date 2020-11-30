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

// ------------------------------------------------------------------------
// ImageVK

ImageVK::ImageVK(PxFormat format,
                 Size2 size,
                 uint32_t layers,
                 uint32_t levels,
                 Samples samples)
  : Image(format, size, layers, levels, samples), owned_(true) {

  if (size == 0)
    throw invalid_argument("ImageVK requires size != 0");
  if (layers == 0)
    throw invalid_argument("ImageVK requires layers != 0");
  if (levels == 0)
    throw invalid_argument("ImageVK requires levels != 0");

  // Convert to format
  VkFormat fmt = toFormatVK(format);
  if (fmt == VK_FORMAT_UNDEFINED)
    throw invalid_argument("ImageVK requires a valid format");

  // Convert to sample count
  VkSampleCountFlagBits smpl = toSampleCountVK(samples);

  // Set image type
  if (size.height > 1)
    type_ = VK_IMAGE_TYPE_2D;
  else
    type_ = VK_IMAGE_TYPE_1D;

  // Get format properties
  auto phys = DeviceVK::get().physicalDev();
  VkFormatProperties fmtProp;
  vkGetPhysicalDeviceFormatProperties(phys, fmt, &fmtProp);

  // Set valid usage mask for use with `tiling`
  auto setUsage = [&](VkImageTiling tiling) {
    usage_ = 0;
    VkFormatFeatureFlags feat;
    if (tiling == VK_IMAGE_TILING_LINEAR)
      feat = fmtProp.linearTilingFeatures;
    else
      feat = fmtProp.optimalTilingFeatures;

    if (feat & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)
      usage_ |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if (feat & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)
      usage_ |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (feat & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
      usage_ |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    // XXX: this check assumes that multisample storage is not supported,
    // since it could spoil additional capabilities query otherwise
    if (samples == Samples1 && (feat & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT))
      usage_ |= VK_IMAGE_USAGE_STORAGE_BIT;

    if (usage_ == 0)
      return false;

    if (DeviceVK::get().devVersion() >= VK_API_VERSION_1_1) {
      if (feat & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT)
        usage_ |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      if (feat & VK_FORMAT_FEATURE_TRANSFER_DST_BIT)
        usage_ |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    } else {
      // XXX: not in v1.0
      usage_ |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    return true;
  };

  // Set image tiling and layout properties
  auto setTiling = [&](VkImageTiling tiling) {
    VkImageFormatProperties prop;
    auto res = vkGetPhysicalDeviceImageFormatProperties(phys, fmt, type_,
                                                        tiling, usage_, 0,
                                                        &prop);
    switch (res) {
    case VK_SUCCESS:
      if (prop.maxExtent.width < size.width ||
          prop.maxExtent.height < size.height ||
          prop.maxMipLevels < levels ||
          prop.maxArrayLayers < layers ||
          !(prop.sampleCounts & smpl))
        return false;

      tiling_ = tiling;
      if (tiling == VK_IMAGE_TILING_LINEAR)
        layout_ = nextLayout_ = VK_IMAGE_LAYOUT_PREINITIALIZED;
      else
        layout_ = nextLayout_ = VK_IMAGE_LAYOUT_UNDEFINED;
      return true;

    case VK_ERROR_FORMAT_NOT_SUPPORTED:
      return false;

    default:
      throw DeviceExcept("Could not query image format properties");
    }
  };

  // Check if image creation with linear tiling _might_ work
  if (samples != Samples1 ||
      !setUsage(VK_IMAGE_TILING_LINEAR) ||
      !setTiling(VK_IMAGE_TILING_LINEAR)) {

    // No chance of linear tiling working, try optimal tiling
    if (!setUsage(VK_IMAGE_TILING_OPTIMAL) ||
        !setTiling(VK_IMAGE_TILING_OPTIMAL))
      throw UnsupportedExcept("Format not supported by ImageVK");
  }

  // Create image
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

  // Allocate/bind/map memory
  VkMemoryRequirements memReq;
  vkGetImageMemoryRequirements(dev, handle_, &memReq);

  try {
    memory_ = allocateVK(memReq, tiling_ == VK_IMAGE_TILING_LINEAR);
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

ImageVK::ImageVK(PxFormat format,
                 Size2 size,
                 uint32_t layers,
                 uint32_t levels,
                 Samples samples,
                 VkImageType type,
                 VkImageTiling tiling,
                 VkImageUsageFlags usage,
                 VkImage handle,
                 void* data,
                 VkImageLayout layout,
                 bool owned)
  : Image(format, size, layers, levels, samples), owned_(owned),
    type_(type), tiling_(tiling), usage_(usage), handle_(handle),
    data_(data), layout_(layout), nextLayout_(layout) {

  if (size == 0)
    throw invalid_argument("ImageVK requires size != 0");
  if (layers == 0)
    throw invalid_argument("ImageVK requires layers != 0");
  if (levels == 0)
    throw invalid_argument("ImageVK requires levels != 0");
  if (toFormatVK(format) == VK_FORMAT_UNDEFINED)
    throw invalid_argument("ImageVK requires a valid format");
  if (handle == VK_NULL_HANDLE)
    throw invalid_argument("ImageVK requires a valid handle");
  if (tiling == VK_IMAGE_TILING_LINEAR && !data)
    throw invalid_argument("ImageVK linear tiling requires non-null data");
}

ImageVK::~ImageVK() {
  // TODO: notify
  if (owned_) {
    auto dev = DeviceVK::get().device();
    vkDestroyImage(dev, handle_, nullptr);
    deallocateVK(memory_);
  }
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
    // For linear tiling, just query subresource layout and then write
    // contents to memory (through `data_` pointer) directly

    // Query subresource layout
    VkImageSubresource subres;
    subres.aspectMask = aspectOfVK(format_);
    subres.mipLevel = level;
    subres.arrayLayer = layer;

    if (subres.aspectMask != VK_IMAGE_ASPECT_COLOR_BIT &&
        subres.aspectMask != VK_IMAGE_ASPECT_DEPTH_BIT &&
        subres.aspectMask != VK_IMAGE_ASPECT_STENCIL_BIT)
      throw runtime_error("Invalid aspect mask for image write");

    auto dev = DeviceVK::get().device();
    VkSubresourceLayout layout;
    // TODO: consider getting all required layouts once on creation
    vkGetImageSubresourceLayout(dev, handle_, &subres, &layout);

    // Write data to image memory
    const auto len = (bitsPerTexel_ >> 3) * size.width;
    auto src = reinterpret_cast<const uint8_t*>(data);
    auto dst = reinterpret_cast<uint8_t*>(data_);
    dst += layout.offset + layout.arrayPitch*layer;
    dst += offset.y * layout.rowPitch + offset.x * (bitsPerTexel_ >> 3);

    for (uint32_t row = 0; row < size.height; ++row) {
      memcpy(dst, src, len);
      dst += layout.rowPitch;
      src += len;
    }

  } else {
    // For optimal tiling, create a staging buffer into which the data
    // will be written and then issue a buffer-to-image copy command

    const uint32_t txSz = (bitsPerTexel_ >> 3);
    BufferVK* buf = nullptr;

    // One staging buffer per layer
    if (staging_.find(layer) == staging_.end()) {
      uint64_t sz = size_.width * size_.height * txSz;
      sz = (sz & ~255) + 256;
      if (levels_ > 1)
        sz <<= 1;
      VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      auto tp = staging_.emplace(layer, make_unique<BufferVK>(sz, usage));
      buf = tp.first->second.get();
    }

    // The mipmap chain is stored contiguosly in the buffer
    uint64_t off = 0;
    for (uint32_t i = 0; i < level; ++i) {
      off += (size_.width >> i) * (size_.height >> i) * txSz;
      off = (off-1 & ~3) + 4;
    }
    if (offset != 0)
      off += offset.y * (size_.width >> level) * txSz + offset.x * txSz;
    uint64_t sz = (size.width >> level) * (size.height >> level) * txSz;

    // TODO: might want to check if write area falls inside the level bounds

    // Write data to staging buffer
    buf->write(off, sz, data);

    // Get priority buffer into which the transfer will be encoded
    // TODO: improve staging buffer management
    auto& queue = static_cast<QueueVK&>(DeviceVK::get().defaultQueue());
    auto cbuf = queue.getPriority(VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                  [&](bool) { staging_.clear(); });

    // Encode transfer command
    VkBufferImageCopy region;
    region.bufferOffset = off;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource = {aspectOfVK(format_), level, layer, 1};
    region.imageOffset = {offset.x, offset.y, 1};
    region.imageExtent = {size.width, size.height, 1};

    vkCmdCopyBufferToImage(cbuf, buf->handle(), handle_, nextLayout_,
                           1, &region);
  }
}

void ImageVK::changeLayout(VkImageLayout newLayout, bool defer) {
  if (nextLayout_ == newLayout)
    return;

  if (layout_ != nextLayout_)
    throw runtime_error("Multiple layout transitions requested");

  VkImageMemoryBarrier barrier;
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.pNext = nullptr;
  barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrier.oldLayout = layout_;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = handle_;
  barrier.subresourceRange.aspectMask = aspectOfVK(format_);
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = levels_;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = layers_;

  barrier_ = barrier;
  changeLayout(defer);
}

void ImageVK::changeLayout(const VkImageMemoryBarrier& barrier, bool defer) {
  if (nextLayout_ == barrier.newLayout)
    return;

  if (layout_ != nextLayout_)
    throw runtime_error("Multiple layout transitions requested");

  barrier_ = barrier;
  changeLayout(defer);
}

void ImageVK::changeLayout(bool defer) {
  nextLayout_ = barrier_.newLayout;

  // TODO: improve this
  VkPipelineStageFlags srcMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  VkPipelineStageFlags dstMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

  auto& queue = static_cast<QueueVK&>(DeviceVK::get().defaultQueue());
  auto cbuf = queue.getPriority(dstMask, [&](bool result) {
    if (result)
      layout_ = nextLayout_;
    else
      nextLayout_ = layout_;
  });

  vkCmdPipelineBarrier(cbuf, srcMask, dstMask, 0, 0, nullptr, 0, nullptr,
                       1, &barrier_);
  if (!defer)
    queue.submit();
}

void ImageVK::layoutChanged(VkImageLayout newLayout) {
  if (layout_ != nextLayout_)
    throw runtime_error("Bad layout transition");

  layout_ = nextLayout_ = newLayout;
}

VkImage ImageVK::handle() const {
  return handle_;
}

pair<VkImageLayout, VkImageLayout> ImageVK::layout() const {
  return {layout_, nextLayout_};
}

ImageVK::View::Ptr ImageVK::getView(uint32_t firstLayer,
                                    uint32_t layerCount,
                                    uint32_t firstLevel,
                                    uint32_t levelCount) {

  if (layerCount == 0 || firstLayer + layerCount > layers_ ||
      levelCount == 0 || firstLevel + levelCount > levels_)
    throw invalid_argument("ImageVK getView()");

  VkImageViewCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.image = handle_;
  info.format = toFormatVK(format_);

  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  info.subresourceRange.aspectMask = aspectOfVK(format_);
  info.subresourceRange.baseMipLevel = firstLevel;
  info.subresourceRange.levelCount = levelCount;
  info.subresourceRange.baseArrayLayer = firstLayer;
  info.subresourceRange.layerCount = layerCount;

  if (type_ == VK_IMAGE_TYPE_2D) {
    if (layerCount == 1)
      info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    else
      info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
  } else {
    if (layerCount == 1)
      info.viewType = VK_IMAGE_VIEW_TYPE_1D;
    else
      info.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
  }

  VkImageView iv;
  auto res = vkCreateImageView(DeviceVK::get().device(), &info, nullptr, &iv);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create image view");

  //views_.emplace(iv, 0).first->second++;
  return make_unique<View>(*this, iv, firstLayer, layerCount,
                           firstLevel, levelCount);
}

ImageVK::View::View(ImageVK& image,
                    VkImageView handle,
                    uint32_t firstLayer,
                    uint32_t layerCount,
                    uint32_t firstLevel,
                    uint32_t levelCount)
  : image_(image), handle_(handle),
    firstLayer_(firstLayer), layerCount_(layerCount),
    firstLevel_(firstLevel), levelCount_(levelCount) { }

ImageVK::View::~View() {
  // [1.2.146 c2.3]
  // "Objects of a non-dispatchable type may not have unique handle values
  // within a type or across types. If handle values are not unique, then
  // destroying one such handle must not cause identical handles of other
  // types to become invalid, and must not cause identical handles of the
  // same type to become invalid if that handle value has been created
  // more times than it has been destroyed".

  vkDestroyImageView(DeviceVK::get().device(), handle_, nullptr);

//  auto it = image_.views_.find(handle_);
//  if (it == image_.views_.end())
//    return;
//
//  if (it->second > 1) {
//    it->second--;
//  } else {
//    // XXX: one must ensure this resource is not in use
//    image_.views_.erase(it);
//    vkDestroyImageView(DeviceVK::get().device(), handle_, nullptr);
//  }
}

ImageVK& ImageVK::View::image() const {
  return image_;
}

VkImageView ImageVK::View::handle() const {
  return handle_;
}

uint32_t ImageVK::View::firstLayer() const {
  return firstLayer_;
}

uint32_t ImageVK::View::layerCount() const {
  return layerCount_;
}

uint32_t ImageVK::View::firstLevel() const {
  return firstLevel_;
}

uint32_t ImageVK::View::levelCount() const {
  return levelCount_;
}

// ------------------------------------------------------------------------
// SamplerVK

SamplerVK::Ptr SamplerVK::make(ImgSampler type) {
  return Ptr(new SamplerVK(type));
}

SamplerVK::SamplerVK(ImgSampler type) : type_(type) {
  VkSamplerCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  // TODO: make additional types for setting these parameters
  info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  info.mipLodBias = 0.0f;
  info.anisotropyEnable = false;
  info.maxAnisotropy = 0.0f;
  info.compareEnable = false;
  info.compareOp = VK_COMPARE_OP_NEVER;
  info.minLod = 0.0f;
  info.maxLod = 0.0f;
  info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
  info.unnormalizedCoordinates = false;

  switch (type) {
  case ImgSamplerBasic:
    info.magFilter = VK_FILTER_NEAREST;
    info.minFilter = VK_FILTER_NEAREST;
    info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    break;
  case ImgSamplerLinear:
    info.magFilter = VK_FILTER_LINEAR;
    info.minFilter = VK_FILTER_LINEAR;
    info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    break;
  case ImgSamplerTrilinear:
    info.magFilter = VK_FILTER_LINEAR;
    info.minFilter = VK_FILTER_LINEAR;
    info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    break;
  }

  auto dev = DeviceVK::get().device();
  auto res = vkCreateSampler(dev, &info, nullptr, &handle_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create sampler");
}

SamplerVK::~SamplerVK() {
  // XXX: like the image view above, assume the driver does reference counting
  vkDestroySampler(DeviceVK::get().device(), handle_, nullptr);
}

ImgSampler SamplerVK::type() const {
  return type_;
}

VkSampler SamplerVK::handle() const {
  return handle_;
}
