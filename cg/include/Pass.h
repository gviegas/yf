//
// cg
// Pass.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_PASS_H
#define YF_CG_PASS_H

#include <cstdint>
#include <vector>
#include <memory>

#include "yf/cg/Defs.h"
#include "yf/cg/Types.h"
#include "yf/cg/Result.h"
#include "yf/cg/Image.h"

CG_NS_BEGIN

/// Initial operation for an attachment.
///
enum LoadOp {
  LoadOpLoad,
  LoadOpClear,
  LoadOpDontCare
};

/// Final operation for an attachment.
///
enum StoreOp {
  StoreOpStore,
  StoreOpDontCare
};

/// Configuration for color attachments.
///
struct ColorAttach {
  PxFormat format;
  Samples samples;
  LoadOp loadOp;
  StoreOp storeOp;
};

/// Configuration for depth/stencil attachments.
///
struct DepStenAttach {
  PxFormat format;
  Samples samples;
  LoadOp depLoadOp;
  StoreOp depStoreOp;
  LoadOp stenLoadOp;
  StoreOp stenStoreOp;
};

/// Attachment resource.
///
struct AttachImg {
  Image* image;
  uint32_t baseLayer;
};

class Pass;

/// Render target.
///
class Target {
 public:
  using Ptr = std::unique_ptr<Target>;
  Target() = default;
  virtual ~Target() = default;

  /// The pass that created this target.
  ///
  virtual const Pass& pass() const = 0;
};

/// Render pass.
///
class Pass {
 public:
  using Ptr = std::unique_ptr<Pass>;

  Pass(const std::vector<ColorAttach>* colors,
       const std::vector<ColorAttach>* resolves,
       const DepStenAttach* depthStencil);

  virtual ~Pass();

  /// Makes a new target compatible with this pass.
  ///
  virtual Target::Ptr makeTarget(Size2 size,
                                 uint32_t layers,
                                 const std::vector<AttachImg>* colors,
                                 const std::vector<AttachImg>* resolves,
                                 const AttachImg* depthStencil) = 0;

  /// Instance constants.
  ///
  using ColorsPtr  = std::unique_ptr<std::vector<ColorAttach>>;
  using DepStenPtr = std::unique_ptr<DepStenAttach>;
  const ColorsPtr  colors;
  const ColorsPtr  resolves;
  const DepStenPtr depthStencil;
};

CG_NS_END

#endif // YF_CG_PASS_H
