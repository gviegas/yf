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

/// Converts from a `CmpFunc` value.
///
inline VkCompareOp toCompareOpVK(CmpFunc cmpFunc) {
  switch (cmpFunc) {
  case CmpFunc::Never:        return VK_COMPARE_OP_NEVER;
  case CmpFunc::Less:         return VK_COMPARE_OP_LESS;
  case CmpFunc::Equal:        return VK_COMPARE_OP_EQUAL;
  case CmpFunc::LessEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
  case CmpFunc::Greater:      return VK_COMPARE_OP_GREATER;
  case CmpFunc::NotEqual:     return VK_COMPARE_OP_NOT_EQUAL;
  case CmpFunc::GreaterEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
  case CmpFunc::Always:       return VK_COMPARE_OP_ALWAYS;

  case CmpFunc::None:
  default:
    throw std::invalid_argument(__func__);
  }
}

CG_NS_END

#endif // YF_CG_SAMPLERVK_H
