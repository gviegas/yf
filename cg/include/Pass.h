//
// CG
// Pass.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_PASS_H
#define YF_CG_PASS_H

#include <cstdint>
#include <memory>
#include <vector>

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

  virtual ~Target();

  /// Getters.
  ///
  virtual Pass& pass() = 0;
  virtual Size2 size() const = 0;
  virtual uint32_t layers() const = 0;
  virtual const std::vector<AttachImg>* colors() const = 0;
  virtual const std::vector<AttachImg>* resolves() const = 0;
  virtual const AttachImg* depthStencil() const = 0;
};

/// Render pass.
///
class Pass {
 public:
  using Ptr = std::unique_ptr<Pass>;

  virtual ~Pass();

  /// Creates a new target compatible with this pass.
  ///
  virtual Target::Ptr target(Size2 size, uint32_t layers,
                             const std::vector<AttachImg>* colors,
                             const std::vector<AttachImg>* resolves,
                             const AttachImg* depthStencil) = 0;

  /// Getters.
  ///
  virtual const std::vector<ColorAttach>* colors() const = 0;
  virtual const std::vector<ColorAttach>* resolves() const = 0;
  virtual const DepStenAttach* depthStencil() const = 0;
};

CG_NS_END

#endif // YF_CG_PASS_H
