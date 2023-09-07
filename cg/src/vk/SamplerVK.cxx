//
// CG
// SamplerVK.cxx
//
// Copyright © 2023 Gustavo C. Viegas.
//

#include <algorithm>

#include "SamplerVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

SamplerVK::SamplerVK(const Desc& desc) : Sampler(desc) {
  auto& dev = deviceVK();
  const auto& lim = dev.physLimits();

  // TODO: Need to enable the sampler anisotropy feature.
  const auto clampedAniso = min(static_cast<float>(1/*maxAnisotropy()*/),
                                lim.maxSamplerAnisotropy);

  const auto cmp = desc.compare == CmpFunc::None ? VK_FALSE : VK_TRUE;
  const auto cmpOp = cmp == VK_TRUE ? toCompareOpVK(desc.compare)
                                    : VK_COMPARE_OP_NEVER;

  VkSamplerCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.magFilter = toFilterVK(magFilter());
  info.minFilter = toFilterVK(minFilter());
  info.mipmapMode = toMipmapModeVK(mipFilter());
  info.addressModeU = toAddressModeVK(wrapModeU());
  info.addressModeV = toAddressModeVK(wrapModeV());
  info.addressModeW = toAddressModeVK(wrapModeW());
  info.mipLodBias = 0.0f;
  info.anisotropyEnable = VK_FALSE; // TODO
  info.maxAnisotropy = clampedAniso;
  info.compareEnable = cmp;
  info.compareOp = cmpOp;
  info.minLod = desc.lodMinClamp;
  info.maxLod = desc.lodMaxClamp;
  info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
  info.unnormalizedCoordinates = VK_FALSE;

  const auto res = vkCreateSampler(dev.device(), &info, nullptr, &handle_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create sampler");
};

SamplerVK::~SamplerVK() {
  vkDestroySampler(deviceVK().device(), handle_, nullptr);
}

VkSampler SamplerVK::handle() {
  return handle_;
}
