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

/// Attachment description.
///
struct AttachDesc {
  PxFormat format;
  Samples samples;
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

  Target() = default;
  Target(const Target&) = delete;
  Target& operator=(const Target&) = delete;
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

  Pass() = default;
  Pass(const Pass&) = delete;
  Pass& operator=(const Pass&) = delete;
  virtual ~Pass();

  /// Creates a new target compatible with the pass.
  ///
  virtual Target::Ptr target(Size2 size, uint32_t layers,
                             const std::vector<AttachImg>* colors,
                             const std::vector<AttachImg>* resolves,
                             const AttachImg* depthStencil) = 0;

  /// Getters.
  ///
  virtual const std::vector<AttachDesc>* colors() const = 0;
  virtual const std::vector<AttachDesc>* resolves() const = 0;
  virtual const AttachDesc* depthStencil() const = 0;
};

CG_NS_END

#endif // YF_CG_PASS_H
