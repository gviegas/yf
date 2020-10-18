//
// yf
// CGState.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_STATE_H
#define YF_CG_STATE_H

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <memory>

#include "YFDefs.h"

YF_NS_BEGIN

/// Vertex formats.
///
enum CGVxFormat {
  CGVxFormatInt, CGVxFormatInt2, CGVxFormatInt3, CGVxFormatInt4,
  CGVxFormatInt2x2, CGVxFormatInt2x3, CGVxFormatInt2x4,
  CGVxFormatInt3x2, CGVxFormatInt3x3, CGVxFormatInt3x4,
  CGVxFormatInt4x2, CGVxFormatInt4x3, CGVxFormatInt4x4,

  CGVxFormatUint, CGVxFormatUint2, CGVxFormatUint3, CGVxFormatUint4,
  CGVxFormatUint2x2, CGVxFormatUint2x3, CGVxFormatUint2x4,
  CGVxFormatUint3x2, CGVxFormatUint3x3, CGVxFormatUint3x4,
  CGVxFormatUint4x2, CGVxFormatUint4x3, CGVxFormatUint4x4,

  CGVxFormatFlt, CGVxFormatFlt2, CGVxFormatFlt3, CGVxFormatFlt4,
  CGVxFormatFlt2x2, CGVxFormatFlt2x3, CGVxFormatFlt2x4,
  CGVxFormatFlt3x2, CGVxFormatFlt3x3, CGVxFormatFlt3x4,
  CGVxFormatFlt4x2, CGVxFormatFlt4x3, CGVxFormatFlt4x4,

  CGVxFormatDbl, CGVxFormatDbl2, CGVxFormatDbl3, CGVxFormatDbl4,
  CGVxFormatDbl2x2, CGVxFormatDbl2x3, CGVxFormatDbl2x4,
  CGVxFormatDbl3x2, CGVxFormatDbl3x3, CGVxFormatDbl3x4,
  CGVxFormatDbl4x2, CGVxFormatDbl4x3, CGVxFormatDbl4x4
};

/// Vertex attribute.
///
struct CGVxAttr {
  CGVxFormat format;
  uint64_t offset;
};

/// Vertex step function.
///
enum CGVxStepFn {
  CGVxStepFnVertex,
  CGVxStepFnInstance
};

using CGVxId = uint32_t;
using CGVxAttrs = std::unordered_map<CGVxId, CGVxAttr>;

/// Collection of vertex attributes using the same buffer binding.
///
struct CGVxInput {
  CGVxAttrs attributes;
  uint32_t stride;
  CGVxStepFn stepFunction;
};

/// Type of a graphics primitive.
///
enum CGPrimitive {
  CGPrimitivePoint,
  CGPrimitiveLine,
  CGPrimitiveTriangle
  // TODO
};

/// Polygon modes.
///
enum CGPolyMode {
  CGPolyModeFill,
  CGPolyModeLine,
  CGPolyModePoint
};

/// Cull modes.
///
enum CGCullMode {
  CGCullModeNone,
  CGCullModeFront,
  CGCullModeBack,
  CGCullModeAny
};

/// Directions to determine front/back-facing primitives.
///
enum CGWinding {
  CGWindingClockwise,
  CGWindingCounterCw
};

class CGPass;
class CGShader;
class CGDcTable;

/// Graphics state.
///
class CGGrState {
 public:
  using Ptr = std::unique_ptr<CGGrState>;

  /// Configuration that determines how the state operates.
  ///
  struct Config {
    CGPass* pass;
    std::vector<CGShader*> shaders;
    std::vector<CGDcTable*> dcTables;
    std::vector<CGVxInput> vxInputs;
    CGPrimitive primitive;
    CGPolyMode polyMode;
    CGCullMode cullMode;
    CGWinding winding;
  };

  explicit CGGrState(const Config& config);
  explicit CGGrState(Config&& config);
  virtual ~CGGrState() = 0;

  /// The state configuration.
  ///
  const Config config;
};

/// Compute state.
///
class CGCpState {
 public:
  using Ptr = std::unique_ptr<CGCpState>;

  /// Configuration that determines how the state operates.
  ///
  struct Config {
    CGShader* shader;
    std::vector<CGDcTable*> dcTables;
  };

  explicit CGCpState(const Config& config);
  explicit CGCpState(Config&& config);
  virtual ~CGCpState() = 0;

  /// The state configuration.
  ///
  const Config config;
};

YF_NS_END

#endif // YF_CG_STATE_H
