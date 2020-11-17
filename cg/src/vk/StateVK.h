//
// CG
// StateVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_STATEVK_H
#define YF_CG_STATEVK_H

#include "State.h"
#include "VK.h"

CG_NS_BEGIN

class GrStateVK final : public GrState {
 public:
  explicit GrStateVK(const Config& config);
  ~GrStateVK();

  /// Getter.
  ///
  VkPipeline pipeline() const;

 private:
  VkShaderStageFlags stgFlags_ = 0;
  VkPipelineLayout plLayout_ = VK_NULL_HANDLE;
  VkPipeline pipeline_ = VK_NULL_HANDLE;
};

class CpStateVK final : public CpState {
 public:
  explicit CpStateVK(const Config& config);
  ~CpStateVK();

  /// Getter.
  ///
  VkPipeline pipeline() const;

 private:
  VkPipelineLayout plLayout_ = VK_NULL_HANDLE;
  VkPipeline pipeline_ = VK_NULL_HANDLE;
};

/// Converts from a `Primitive` value.
///
inline VkPrimitiveTopology toTopologyVK(Primitive primitive) {
  switch (primitive) {
  case PrimitivePoint:    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
  case PrimitiveLine:     return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  case PrimitiveTriangle: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  }
}

/// Converts from a `VxStepFn` value.
///
inline VkVertexInputRate toInputRateVK(VxStepFn stepFn) {
  switch (stepFn) {
  case VxStepFnVertex:   return VK_VERTEX_INPUT_RATE_VERTEX;
  case VxStepFnInstance: return VK_VERTEX_INPUT_RATE_INSTANCE;
  }
}

/// Converts from a `VxFormat` value.
///
inline VkFormat toFormatVK(VxFormat vxFormat) {
  switch (vxFormat) {
  case VxFormatInt:
    return VK_FORMAT_R32_SINT;
  case VxFormatInt2:
  case VxFormatInt2x2:
  case VxFormatInt3x2:
  case VxFormatInt4x2:
    return VK_FORMAT_R32G32_SINT;
  case VxFormatInt3:
  case VxFormatInt2x3:
  case VxFormatInt3x3:
  case VxFormatInt4x3:
    return VK_FORMAT_R32G32B32_SINT;
  case VxFormatInt4:
  case VxFormatInt2x4:
  case VxFormatInt3x4:
  case VxFormatInt4x4:
    return VK_FORMAT_R32G32B32A32_SINT;

  case VxFormatUint:
    return VK_FORMAT_R32_UINT;
  case VxFormatUint2:
  case VxFormatUint2x2:
  case VxFormatUint3x2:
  case VxFormatUint4x2:
    return VK_FORMAT_R32G32_UINT;
  case VxFormatUint3:
  case VxFormatUint2x3:
  case VxFormatUint3x3:
  case VxFormatUint4x3:
    return VK_FORMAT_R32G32B32_UINT;
  case VxFormatUint4:
  case VxFormatUint2x4:
  case VxFormatUint3x4:
  case VxFormatUint4x4:
    return VK_FORMAT_R32G32B32A32_UINT;

  case VxFormatFlt:
    return VK_FORMAT_R32_SFLOAT;
  case VxFormatFlt2:
  case VxFormatFlt2x2:
  case VxFormatFlt3x2:
  case VxFormatFlt4x2:
    return VK_FORMAT_R32G32_SFLOAT;
  case VxFormatFlt3:
  case VxFormatFlt2x3:
  case VxFormatFlt3x3:
  case VxFormatFlt4x3:
    return VK_FORMAT_R32G32B32_SFLOAT;
  case VxFormatFlt4:
  case VxFormatFlt2x4:
  case VxFormatFlt3x4:
  case VxFormatFlt4x4:
    return VK_FORMAT_R32G32B32A32_SFLOAT;

  case VxFormatDbl:
    return VK_FORMAT_R64_SFLOAT;
  case VxFormatDbl2:
  case VxFormatDbl2x2:
  case VxFormatDbl3x2:
  case VxFormatDbl4x2:
    return VK_FORMAT_R64G64_SFLOAT;
  case VxFormatDbl3:
  case VxFormatDbl2x3:
  case VxFormatDbl3x3:
  case VxFormatDbl4x3:
    return VK_FORMAT_R64G64B64_SFLOAT;
  case VxFormatDbl4:
  case VxFormatDbl2x4:
  case VxFormatDbl3x4:
  case VxFormatDbl4x4:
    return VK_FORMAT_R64G64B64A64_SFLOAT;
  }
}

CG_NS_END

#endif // YF_CG_STATEVK_H
