//
// yf
// CGPass.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_PASS_H
#define YF_CG_PASS_H

#include <cstdint>
#include <vector>
#include <memory>

#include "YFDefs.h"
#include "CGTypes.h"
#include "CGResult.h"
#include "CGImage.h"

YF_NS_BEGIN

/// Initial operation for an attachment.
///
enum CGLoadOp {
  CGLoadOpLoad,
  CGLoadOpClear,
  CGLoadOpDontCare
};

/// Final operation for an attachment.
///
enum CGStoreOp {
  CGStoreOpStore,
  CGStoreOpDontCare
};

/// Configuration for color attachments.
///
struct CGColorAttach {
  CGPxFormat format;
  CGSamples samples;
  CGLoadOp loadOp;
  CGStoreOp storeOp;
};

/// Configuration for depth/stencil attachments.
///
struct CGDepStenAttach {
  CGPxFormat format;
  CGSamples samples;
  CGLoadOp depLoadOp;
  CGStoreOp depStoreOp;
  CGLoadOp stenLoadOp;
  CGStoreOp stenStoreOp;
};

/// Attachment resource.
///
struct CGAttachImg {
  CGImage* image;
  uint32_t baseLayer;
};

class CGPass;

/// Render target.
///
class CGTarget {
 public:
  using Ptr = std::unique_ptr<CGTarget>;
  CGTarget() = default;
  virtual ~CGTarget() = default;

  /// The pass that created this target.
  ///
  virtual const CGPass& pass() const = 0;
};

/// Render pass.
///
class CGPass {
 public:
  using Ptr = std::unique_ptr<CGPass>;

  CGPass(const std::vector<CGColorAttach>* colors,
         const std::vector<CGColorAttach>* resolves,
         const CGDepStenAttach* depthStencil);

  virtual ~CGPass();

  /// Makes a new target compatible with this pass.
  ///
  virtual CGTarget::Ptr makeTarget(CGSize2 size,
                                   uint32_t layers,
                                   const std::vector<CGAttachImg>* colors,
                                   const std::vector<CGAttachImg>* resolves,
                                   const CGAttachImg* depthStencil) = 0;

  /// Instance constants.
  ///
  using ColorsPtr  = std::unique_ptr<std::vector<CGColorAttach>>;
  using DepStenPtr = std::unique_ptr<CGDepStenAttach>;
  const ColorsPtr  colors;
  const ColorsPtr  resolves;
  const DepStenPtr depthStencil;
};

YF_NS_END

#endif // YF_CG_PASS_H
