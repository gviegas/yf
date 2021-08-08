//
// CG
// Limits.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_CG_LIMITS_H
#define YF_CG_LIMITS_H

#include <cstdint>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

/// Limits.
///
/// TODO: Other limits.
///
struct Limits {
  uint32_t maxDrawIndex;
  uint32_t maxDispatchWidth;
  uint32_t maxDispatchHeight;
  uint32_t maxDispatchDepth;

  uint32_t maxViewports;
  float maxViewportWidth;
  float maxViewportHeight;

  uint32_t maxImageWidth;
  uint32_t maxImageHeight;
  uint32_t maxImageLayers;

  uint32_t maxPassColors;
  uint32_t maxTargetWidth;
  uint32_t maxTargetHeight;
  uint32_t maxTargetLayers;

  uint32_t maxDcUniform;
  uint32_t maxDcStorage;
  uint32_t maxDcImage;
  uint32_t maxDcImgSampler;
  uint64_t minDcUniformWriteAlignedOffset;
  uint64_t maxDcUniformWriteSize;
  uint64_t minDcStorageWriteAlignedOffset;
  uint64_t maxDcStorageWriteSize;

  uint32_t maxVxInputs;
  uint32_t maxVxAttrs;
};

CG_NS_END

#endif // YF_CG_LIMITS_H
