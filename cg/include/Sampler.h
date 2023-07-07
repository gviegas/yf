//
// CG
// Sampler.h
//
// Copyright © 2023 Gustavo C. Viegas.
//

#ifndef YF_CG_SAMPLER_H
#define YF_CG_SAMPLER_H

#include <cstdint>
#include <memory>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

/// Wrap modes.
///
enum class WrapMode {
  ClampToEdge,
  Repeat,
  MirrorRepeat
};

/// Min/mag filters.
///
enum class Filter {
  Nearest,
  Linear
};

/// Mipmap filters.
///
enum class MipFilter {
  Nearest,
  Linear
};

/// Compare functions.
///
enum class CmpFunc {
  None
  // TODO...
};

class Sampler {
 public:
  using Ptr = std::unique_ptr<Sampler>;

  /// Sampler descriptor.
  ///
  struct Desc {
    WrapMode wrapModeU;
    WrapMode wrapModeV;
    WrapMode wrapModeW;
    Filter magFilter;
    Filter minFilter;
    MipFilter mipFilter;
    float lodMinClamp;
    float lodMaxClamp;
    CmpFunc compare;
    uint16_t maxAnisotropy;
  };

  Sampler(const Desc& desc);
  Sampler(const Sampler&) = delete;
  Sampler& operator=(const Sampler&) = delete;
  virtual ~Sampler() = 0;

  /// Gets u/v/w wrap modes.
  ///
  WrapMode wrapModeU() const;
  WrapMode wrapModeV() const;
  WrapMode wrapModeW() const;

  /// Gets min/mag/mipmap filters.
  ///
  Filter magFilter() const;
  Filter minFiler() const;
  MipFilter mipFilter() const;

  /// Gets level of detail clamp bounds.
  ///
  float lodMinClamp() const;
  float lodMaxClamp() const;

  /// Gets the compare function.
  ///
  CmpFunc compare() const;

  /// Gets the maximum anisotropy.
  ///
  uint16_t maxAnisotropy() const;

 private:
  const WrapMode wrapModeU_;
  const WrapMode wrapModeV_;
  const WrapMode wrapModeW_;
  const Filter magFilter_;
  const Filter minFilter_;
  const MipFilter mipFilter_;
  const float lodMinClamp_;
  const float lodMaxClamp_;
  const CmpFunc compare_;
  const uint16_t maxAnisotropy_;
};

CG_NS_END

#endif // YF_CG_SAMPLER_H
