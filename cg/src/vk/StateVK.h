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

CG_NS_END

#endif // YF_CG_STATEVK_H
