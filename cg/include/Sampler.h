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
    WrapMode wrapModeU = WrapMode::ClampToEdge;
    WrapMode wrapModeV = WrapMode::ClampToEdge;
    WrapMode wrapModeW = WrapMode::ClampToEdge;
    Filter magFilter = Filter::Nearest;
    Filter minFilter = Filter::Nearest;
    MipFilter mipFilter = MipFilter::Nearest;
    float lodMinClamp = 0.0f;
    float lodMaxClamp = 32.0f;
    CmpFunc compare = CmpFunc::None;
    uint16_t maxAnisotropy = 1;
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
  Filter minFilter() const;
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
