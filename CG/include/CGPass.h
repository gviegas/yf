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

enum CGLoadOp {
  CGLoadOpLoad,
  CGLoadOpClear,
  CGLoadOpDontCare
};

enum CGStoreOp {
  CGStoreOpStore,
  CGStoreOpDontCare
};

struct CGColorAttach {
  CGPxFormat format;
  CGSamples samples;
  CGLoadOp loadOp;
  CGStoreOp storeOp;
};

struct CGDepStenAttach {
  CGPxFormat format;
  CGSamples samples;
  CGLoadOp depLoadOp;
  CGStoreOp depStoreOp;
  CGLoadOp stenLoadOp;
  CGStoreOp stenStoreOp;
};

struct CGAttachImg {
  CGImage* image;
  uint32_t baseLayer;
};

class CGTarget;

class CGPass {
 public:
  CGPass(const std::vector<CGColorAttach>* colors,
         const std::vector<CGColorAttach>* resolves,
         const CGDepStenAttach* depthStencil);

  virtual ~CGPass();

  /// Makes a new target compatible with this pass.
  ///
  using TargetPtr = std::unique_ptr<CGTarget>;
  virtual TargetPtr makeTarget(CGSize2 size,
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
