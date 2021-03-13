//
// CG
// Pass.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_PASS_H
#define YF_CG_PASS_H

#include <cstdint>
#include <vector>
#include <memory>

#include "yf/cg/Defs.h"
#include "yf/cg/Types.h"
#include "yf/cg/Image.h"

CG_NS_BEGIN

/// Initial operation for an attachment.
///
enum LoadOp {
  LoadOpLoad,
  //LoadOpClear,
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
  uint32_t layer;
  uint32_t level;
};

class Pass;

/// Render target.
///
class Target {
 public:
  using Ptr = std::unique_ptr<Target>;

  Target(Size2 size, uint32_t layers, const std::vector<AttachImg>* colors,
         const std::vector<AttachImg>* resolves, const AttachImg* depthStencil);

  virtual ~Target();

  /// Gets the `Pass` object that created the target.
  ///
  virtual Pass& pass() const = 0;

  /// Instance constants.
  ///
  using ImgsPtr = std::unique_ptr<std::vector<AttachImg>>;
  using ImgPtr = std::unique_ptr<AttachImg>;
  const Size2 size_;
  const uint32_t layers_;
  const ImgsPtr colors_;
  const ImgsPtr resolves_;
  const ImgPtr depthStencil_;
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

  /// Creates a new target compatible with this pass.
  ///
  virtual Target::Ptr target(Size2 size, uint32_t layers,
                             const std::vector<AttachImg>* colors,
                             const std::vector<AttachImg>* resolves,
                             const AttachImg* depthStencil) = 0;

  /// Instance constants.
  ///
  using ColorsPtr = std::unique_ptr<std::vector<ColorAttach>>;
  using DepStenPtr = std::unique_ptr<DepStenAttach>;
  const ColorsPtr colors_;
  const ColorsPtr resolves_;
  const DepStenPtr depthStencil_;
};

CG_NS_END

#endif // YF_CG_PASS_H
