//
// CG
// SamplerVK.h
//
// Copyright © 2023 Gustavo C. Viegas.
//

#ifndef YF_CG_SAMPLERVK_H
#define YF_CG_SAMPLERVK_H

#include <stdexcept>

#include "Sampler.h"
#include "VK.h"

CG_NS_BEGIN

class SamplerVK final : public Sampler {
 public:
  SamplerVK(const Desc& desc);
  ~SamplerVK();

  /// Getter.
  ///
  VkSampler handle();

 private:
  VkSampler handle_ = VK_NULL_HANDLE;
};

/// Converts from a `WrapMode` value.
///
inline VkSamplerAddressMode toAddressModeVK(WrapMode wrapMode) {
  switch (wrapMode) {
  case WrapMode::ClampToEdge:  return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  case WrapMode::Repeat:       return VK_SAMPLER_ADDRESS_MODE_REPEAT;
  case WrapMode::MirrorRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `Filter` value.
///
inline VkFilter toFilterVK(Filter filter) {
  switch (filter) {
  case Filter::Nearest: return VK_FILTER_NEAREST;
  case Filter::Linear:  return VK_FILTER_LINEAR;
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `MipFilter` value.
///
inline VkSamplerMipmapMode toMipmapModeVK(MipFilter mipFilter) {
  switch (mipFilter) {
  case MipFilter::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
  case MipFilter::Linear:  return VK_SAMPLER_MIPMAP_MODE_LINEAR;
  default:
    throw std::invalid_argument(__func__);
  }
}

// TODO: `CmpFunc` conversion.

CG_NS_END

#endif // YF_CG_SAMPLERVK_H
