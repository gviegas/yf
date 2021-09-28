//
// CG
// StateVK.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_STATEVK_H
#define YF_CG_STATEVK_H

#include <stdexcept>

#include "State.h"
#include "VK.h"

CG_NS_BEGIN

class GrStateVK final : public GrState {
 public:
  explicit GrStateVK(const Config& config);
  ~GrStateVK();

  /// Getters.
  ///
  VkShaderStageFlags stgFlags() const;
  VkPipelineLayout plLayout();
  VkPipeline pipeline();

 private:
  VkShaderStageFlags stgFlags_ = 0;
  VkPipelineLayout plLayout_ = VK_NULL_HANDLE;
  VkPipeline pipeline_ = VK_NULL_HANDLE;
};

class CpStateVK final : public CpState {
 public:
  explicit CpStateVK(const Config& config);
  ~CpStateVK();

  /// Getters.
  ///
  VkPipelineLayout plLayout();
  VkPipeline pipeline();

 private:
  VkPipelineLayout plLayout_ = VK_NULL_HANDLE;
  VkPipeline pipeline_ = VK_NULL_HANDLE;
};

/// Converts from a `Topology` value.
///
inline VkPrimitiveTopology toTopologyVK(Topology topology) {
  switch (topology) {
  case TopologyPoint:    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
  case TopologyLine:     return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  case TopologyTriangle: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  case TopologyLnStrip:  return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
  case TopologyTriStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
  case TopologyTriFan:   return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `PolyMode` value.
///
inline VkPolygonMode toPolygonModeVK(PolyMode polyMode) {
  switch (polyMode) {
  case PolyModeFill:  return VK_POLYGON_MODE_FILL;
  case PolyModeLine:  return VK_POLYGON_MODE_LINE;
  case PolyModePoint: return VK_POLYGON_MODE_POINT;
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `CullMode` value.
///
inline VkCullModeFlagBits toCullModeVK(CullMode cullMode) {
  switch (cullMode) {
  case CullModeNone:  return VK_CULL_MODE_NONE;
  case CullModeFront: return VK_CULL_MODE_FRONT_BIT;
  case CullModeBack:  return VK_CULL_MODE_BACK_BIT;
  case CullModeAny:   return VK_CULL_MODE_FRONT_AND_BACK;
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `Winding` value.
///
inline VkFrontFace toFrontFaceVK(Winding winding) {
  switch (winding) {
  case WindingClockwise: return VK_FRONT_FACE_CLOCKWISE;
  case WindingCounterCw: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `VxStepFn` value.
///
inline VkVertexInputRate toInputRateVK(VxStepFn stepFn) {
  switch (stepFn) {
  case VxStepFnVertex:   return VK_VERTEX_INPUT_RATE_VERTEX;
  case VxStepFnInstance: return VK_VERTEX_INPUT_RATE_INSTANCE;
  default:
    throw std::invalid_argument(__func__);
  }
}

/// Converts from a `VxFormat` value.
///
inline VkFormat toFormatVK(VxFormat vxFormat) {
  switch (vxFormat) {
  case VxFormatByte:  return VK_FORMAT_R8_SINT;
  case VxFormatByte2: return VK_FORMAT_R8G8_SINT;
  case VxFormatByte3: return VK_FORMAT_R8G8B8_SINT;
  case VxFormatByte4: return VK_FORMAT_R8G8B8A8_SINT;

  case VxFormatByteNorm:  return VK_FORMAT_R8_SNORM;
  case VxFormatByteNorm2: return VK_FORMAT_R8G8_SNORM;
  case VxFormatByteNorm3: return VK_FORMAT_R8G8B8_SNORM;
  case VxFormatByteNorm4: return VK_FORMAT_R8G8B8A8_SNORM;

  case VxFormatUbyte:  return VK_FORMAT_R8_UINT;
  case VxFormatUbyte2: return VK_FORMAT_R8G8_UINT;
  case VxFormatUbyte3: return VK_FORMAT_R8G8B8_UINT;
  case VxFormatUbyte4: return VK_FORMAT_R8G8B8A8_UINT;

  case VxFormatUbyteNorm:  return VK_FORMAT_R8_UNORM;
  case VxFormatUbyteNorm2: return VK_FORMAT_R8G8_UNORM;
  case VxFormatUbyteNorm3: return VK_FORMAT_R8G8B8_UNORM;
  case VxFormatUbyteNorm4: return VK_FORMAT_R8G8B8A8_UNORM;

  case VxFormatShrt:  return VK_FORMAT_R16_SINT;
  case VxFormatShrt2: return VK_FORMAT_R16G16_SINT;
  case VxFormatShrt3: return VK_FORMAT_R16G16B16_SINT;
  case VxFormatShrt4: return VK_FORMAT_R16G16B16A16_SINT;

  case VxFormatShrtNorm:  return VK_FORMAT_R16_SNORM;
  case VxFormatShrtNorm2: return VK_FORMAT_R16G16_SNORM;
  case VxFormatShrtNorm3: return VK_FORMAT_R16G16B16_SNORM;
  case VxFormatShrtNorm4: return VK_FORMAT_R16G16B16A16_SNORM;

  case VxFormatUshrt:  return VK_FORMAT_R16_UINT;
  case VxFormatUshrt2: return VK_FORMAT_R16G16_UINT;
  case VxFormatUshrt3: return VK_FORMAT_R16G16B16_UINT;
  case VxFormatUshrt4: return VK_FORMAT_R16G16B16A16_UINT;

  case VxFormatUshrtNorm:  return VK_FORMAT_R16_UNORM;
  case VxFormatUshrtNorm2: return VK_FORMAT_R16G16_UNORM;
  case VxFormatUshrtNorm3: return VK_FORMAT_R16G16B16_UNORM;
  case VxFormatUshrtNorm4: return VK_FORMAT_R16G16B16A16_UNORM;

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

  default:
    throw std::invalid_argument(__func__);
  }
}

CG_NS_END

#endif // YF_CG_STATEVK_H
