//
// CG
// ImageVK.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#include <cstring>

#include "ImageVK.h"
#include "MemoryVK.h"
#include "QueueVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

//
// ImageVK
//

// TODO: Should move `desc` validation to superclass.
ImageVK::ImageVK(const Image::Desc& desc) : Image(desc), owned_(true) {
  if (size().width == 0 || size().height == 0 || size().depthOrLayers == 0)
    throw invalid_argument("ImageVK requires size != 0");
  if (levels() == 0)
    throw invalid_argument("ImageVK requires levels != 0");

  const VkFormat fmt = toFormatVK(format());
  if (fmt == VK_FORMAT_UNDEFINED)
    throw invalid_argument("ImageVK requires a valid format");

  const VkSampleCountFlagBits spl = toSampleCountVK(samples());

  // Convert and validate dimension
  const auto& lim = deviceVK().physLimits();
  VkImageType type;
  switch (dimension()) {
  case Dim1:
    if (size().width > lim.maxImageDimension1D)
      throw invalid_argument("ImageVK size limit");
    if (size().height != 1)
      throw invalid_argument("ImageVK requires height == 1 for 1D images");
    if (size().depthOrLayers > lim.maxImageArrayLayers)
      throw invalid_argument("ImageVK layer limit");
    type = VK_IMAGE_TYPE_1D;
    break;
  case Dim2:
    if (size().width > lim.maxImageDimension2D ||
        size().height > lim.maxImageDimension2D)
      throw invalid_argument("ImageVK size limit");
    if (size().depthOrLayers > lim.maxImageArrayLayers)
      throw invalid_argument("ImageVK layer limit");
    // TODO: Decide how to handle cubes
    type = VK_IMAGE_TYPE_2D;
    break;
  case Dim3:
    if (size().width > lim.maxImageDimension3D ||
        size().height > lim.maxImageDimension3D ||
        size().depthOrLayers > lim.maxImageDimension3D)
      throw invalid_argument("ImageVK size limit");
    type = VK_IMAGE_TYPE_3D;
    break;
  }

  // Convert usage mask and record required format features
  VkImageUsageFlags usage = 0;
  VkFormatFeatureFlags fmtFeat = 0;
  if (usageMask() & CopySrc) {
    usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (deviceVK().devVersion() >= VK_API_VERSION_1_1)
      fmtFeat |= VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
  }
  if (usageMask() & CopyDst) {
    usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if (deviceVK().devVersion() >= VK_API_VERSION_1_1)
      fmtFeat |= VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
  }
  if (usageMask() & Sampled) {
    usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    fmtFeat |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
               // TODO: Check elsewhere
               VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
  }
  if (usageMask() & Storage) {
    usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    fmtFeat |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
  }
  if (usageMask() & Attachment) {
    switch (aspectOfVK(format())) {
    case VK_IMAGE_ASPECT_COLOR_BIT:
      usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
      fmtFeat |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                 // TODO: Check elsewhere
                 VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
      break;
    case VK_IMAGE_ASPECT_DEPTH_BIT:
    case VK_IMAGE_ASPECT_STENCIL_BIT:
      usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
      fmtFeat |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
      break;
    }
  }

  auto phys = deviceVK().physicalDev();
  VkFormatProperties fmtProp;
  vkGetPhysicalDeviceFormatProperties(phys, fmt, &fmtProp);

  // Set image tiling and layout properties
  auto setTiling = [&](VkImageTiling tiling) {
    const auto feat = tiling == VK_IMAGE_TILING_LINEAR ?
                      fmtProp.linearTilingFeatures :
                      fmtProp.optimalTilingFeatures;
    if ((fmtFeat & feat) != fmtFeat)
      return false;

    VkImageFormatProperties prop;
    const auto res = vkGetPhysicalDeviceImageFormatProperties(
      phys, fmt, type, tiling, usage, 0, &prop);

    switch (res) {
    case VK_SUCCESS:
      if (prop.maxExtent.width < size().width ||
          prop.maxExtent.height < size().height ||
          (dimension() == Dim3 ?
           prop.maxExtent.depth :
           prop.maxArrayLayers) < size().depthOrLayers ||
          prop.maxMipLevels < levels() ||
          !(prop.sampleCounts & spl))
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

  // Prefer linear tiling
  if (samples() != Samples1 || !setTiling(VK_IMAGE_TILING_LINEAR))
    if (!setTiling(VK_IMAGE_TILING_OPTIMAL))
      throw UnsupportedExcept("Format not supported by ImageVK");

  // Create image
  auto dev = deviceVK().device();
  VkResult res;

  VkImageCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.imageType = type;
  info.format = fmt;
  info.extent = {size().width, size().height, dimension() == Dim3 ?
                                              size().depthOrLayers :
                                              1};
  info.mipLevels = levels();
  info.arrayLayers = dimension() == Dim3 ? 1 : size().depthOrLayers;
  info.samples = spl;
  info.tiling = tiling_;
  info.usage = usage;
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

// TODO: Should move `desc` validation to superclass.
ImageVK::ImageVK(const Desc& desc, VkImage handle, void* data,
                 VkImageLayout layout, bool owned)
  : Image(desc), owned_(owned),
    tiling_(data ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL),
    handle_(handle), data_(data), layout_(layout), nextLayout_(layout) {

  // TODO: Validate further
  if (handle_ == VK_NULL_HANDLE)
    throw invalid_argument("ImageVK requires a valid handle");
  if (size().width == 0 || size().height == 0 || size().depthOrLayers == 0)
    throw invalid_argument("ImageVK requires size != 0");
  if (levels() == 0)
    throw invalid_argument("ImageVK requires levels != 0");
  if (toFormatVK(format()) == VK_FORMAT_UNDEFINED)
    throw invalid_argument("ImageVK requires a valid format");
}

ImageVK::~ImageVK() {
  // TODO: Notify
  if (owned_) {
    auto dev = deviceVK().device();
    vkDestroyImage(dev, handle_, nullptr);
    deallocateVK(memory_);
  }
}

// TODO: Manage views' lifetimes
ImgView::Ptr ImageVK::view(const ImgView::Desc& desc) {
  return ImgView::Ptr(new ImgViewVK(*this, desc));
}

void ImageVK::write(uint32_t plane, Origin3 origin, uint32_t level,
                    const void* data, Size3 size, uint32_t bytesPerRow,
                    uint32_t rowsPerSlice) {

  if (origin.x + size.width > this->size().width ||
      origin.y + size.height > this->size().height ||
      origin.z + size.depthOrLayers > this->size().depthOrLayers ||
      level >= levels() ||
      !data)
    throw invalid_argument("ImageVK write()");

  // TODO: Consider storing the aspect as data member
  VkImageAspectFlags aspFlg = aspectOfVK(format());
  switch (aspFlg) {
  case VK_IMAGE_ASPECT_COLOR_BIT:
  case VK_IMAGE_ASPECT_DEPTH_BIT:
  case VK_IMAGE_ASPECT_STENCIL_BIT:
    if (plane != 0)
      throw invalid_argument("ImageVK write() invalid plane");
    break;
  case VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT:
    if (plane == 0)
      aspFlg = VK_IMAGE_ASPECT_DEPTH_BIT;
    else if (plane == 1)
      aspFlg = VK_IMAGE_ASPECT_STENCIL_BIT;
    else
      throw invalid_argument("ImageVK write() invalid plane");
    break;
  }

  const auto txSz = texelSize();

  if (tiling_ == VK_IMAGE_TILING_LINEAR) {
    // For linear tiling, just query subresource layout and then write
    // contents to memory (through `data_` pointer) directly

    // Must be host-visible
    if (layout_ != VK_IMAGE_LAYOUT_PREINITIALIZED &&
        layout_ != VK_IMAGE_LAYOUT_GENERAL)
      changeLayout(VK_IMAGE_LAYOUT_GENERAL, false);

    const auto dev = deviceVK().device();

    // Query subresource layout
    // NOTE: This assumes `format_` has a single aspect
    VkImageSubresource subres;
    subres.aspectMask = aspFlg;
    subres.mipLevel = level;
    subres.arrayLayer = 0;
    VkSubresourceLayout layout;
    vkGetImageSubresourceLayout(dev, handle_, &subres, &layout);

    const auto slcPitch = dimension() == Dim3 ?
                          layout.depthPitch :
                          layout.arrayPitch;

    const auto rowSz = size.width * txSz;

    if (bytesPerRow == 0)
      bytesPerRow = rowSz;
    if (rowsPerSlice == 0)
      rowsPerSlice = size.height;

    // Write data to each selected slice, row by row
    for (uint32_t i = 0; i < size.depthOrLayers; i++) {
      auto dst = reinterpret_cast<char*>(data_) +
                 layout.offset +
                 origin.x * txSz +
                 origin.y * layout.rowPitch +
                 (origin.z + i) * slcPitch;

      auto src = reinterpret_cast<const char*>(data) +
                 bytesPerRow * rowsPerSlice * i;

      for (uint32_t row = 0; row < size.height; row++) {
        memcpy(dst, src, rowSz);
        dst += layout.rowPitch;
        src += bytesPerRow;
      }
    }

  } else {
    // For optimal tiling, write the data to a staging buffer and then
    // issue a buffer-to-image copy command

    // TODO
    throw runtime_error("ImageVK::write() WIP");
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
  barrier.subresourceRange.aspectMask = aspectOfVK(format());
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = levels();
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = size().depthOrLayers; // XXX: 3D

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

  // TODO: Needs improvement
  VkPipelineStageFlags srcMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  VkPipelineStageFlags dstMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

  auto& queue = static_cast<QueueVK&>(deviceVK().defaultQueue());
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

VkImage ImageVK::handle() {
  return handle_;
}

pair<VkImageLayout, VkImageLayout> ImageVK::layout() const {
  return {layout_, nextLayout_};
}

//
// ImgViewVK
//

// TODO: Should move `desc` validation to superclass.
ImgViewVK::ImgViewVK(ImageVK& image, const ImgView::Desc& desc)
  : ImgView(image, desc) {

  if (levels().count() == 0 || levels().count() > image.levels() ||
      layers().count() == 0 || layers().count() > image.size().depthOrLayers)
    throw invalid_argument("ImgViewVK()");

  // Validate image/view compatibility and set view type
  // TODO: Ensure that `image` is cube-compatible when applicable
  // TODO: Validate sample count
  VkImageViewType type;
  switch (dimension()) {
  case Dim1:
    if (image.dimension() != Image::Dim1)
      throw invalid_argument("Image/View dimension mismatch");
    if (layers().count() != 1)
      throw invalid_argument("View dimension/layer count mismatch");
    type = VK_IMAGE_VIEW_TYPE_1D;
    break;

  case Dim1Array:
    if (image.dimension() != Image::Dim1)
      throw invalid_argument("Image/View dimension mismatch");
    type = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    break;

  case Dim2:
    if (image.dimension() != Image::Dim2)
      throw invalid_argument("Image/View dimension mismatch");
    if (layers().count() != 1)
      throw invalid_argument("View dimension/layer count mismatch");
    type = VK_IMAGE_VIEW_TYPE_2D;
    break;

  case Dim2Array:
    if (image.dimension() != Image::Dim2)
      throw invalid_argument("Image/View dimension mismatch");
    type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    break;

  case DimCube:
    if (image.dimension() != Image::Dim2)
      throw invalid_argument("Image/View dimension mismatch");
    if (layers().count() != 6)
      throw invalid_argument("View dimension/layer count mismatch");
    type = VK_IMAGE_VIEW_TYPE_CUBE;
    break;

  case DimCubeArray:
    if (image.dimension() != Image::Dim2)
      throw invalid_argument("Image/View dimension mismatch");
    if (layers().count() % 6)
      throw invalid_argument("View dimension/layer count mismatch");
    type = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    break;

  case Dim3:
    if (image.dimension() != Image::Dim3)
      throw invalid_argument("Image/View dimension mismatch");
    // TODO: Maybe allow 0 layers here
    if (layers().count() != 1)
      throw invalid_argument("View dimension/layer count mismatch");
    type = VK_IMAGE_VIEW_TYPE_3D;
    break;
  }

  // Create view
  VkImageViewCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.image = image.handle();
  info.viewType = type;
  info.format = toFormatVK(image.format());
  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  // TODO: Aspect should be provided by `desc`
  info.subresourceRange.aspectMask = aspectOfVK(image.format());
  info.subresourceRange.baseMipLevel = levels().start;
  info.subresourceRange.levelCount = levels().count();
  info.subresourceRange.baseArrayLayer = layers().start;
  info.subresourceRange.layerCount = layers().count();

  const auto res = vkCreateImageView(deviceVK().device(), &info, nullptr,
                                     &handle_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create image view");
}

ImgViewVK::~ImgViewVK() {
  vkDestroyImageView(deviceVK().device(), handle_, nullptr);
}

VkImageView ImgViewVK::handle() {
  return handle_;
}
