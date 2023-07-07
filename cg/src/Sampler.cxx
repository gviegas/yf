//
// CG
// Sampler.cxx
//
// Copyright © 2023 Gustavo C. Viegas.
//

#include <stdexcept>

#include "Sampler.h"

using namespace CG_NS;
using namespace std;

Sampler::Sampler(const Desc& desc)
  : wrapModeU_(desc.wrapModeU), wrapModeV_(desc.wrapModeV),
    wrapModeW_(desc.wrapModeW), magFilter_(desc.magFilter),
    minFilter_(desc.minFilter), mipFilter_(desc.mipFilter),
    lodMinClamp_(desc.lodMinClamp), lodMaxClamp_(desc.lodMaxClamp),
    compare_(desc.compare), maxAnisotropy_(desc.maxAnisotropy) {

  if (lodMinClamp_ < 0.0f || lodMinClamp_ > lodMaxClamp_)
    throw invalid_argument("Sampler::Desc has invalid level of detail");

  if (maxAnisotropy_ == 0)
    throw invalid_argument("Sampler::Desc::maxAnisotropy must be at least 1");
}

Sampler::~Sampler() { }

WrapMode Sampler::wrapModeU() const {
  return wrapModeU_;
}

WrapMode Sampler::wrapModeV() const {
  return wrapModeV_;
}

WrapMode Sampler::wrapModeW() const {
  return wrapModeW_;
}

Filter Sampler::magFilter() const {
  return magFilter_;
}

Filter Sampler::minFiler() const {
  return minFilter_;
}

MipFilter Sampler::mipFilter() const {
  return mipFilter_;
}

float Sampler::lodMinClamp() const {
  return lodMinClamp_;
}

float Sampler::lodMaxClamp() const {
  return lodMaxClamp_;
}

CmpFunc Sampler::compare() const {
  return compare_;
}

uint16_t Sampler::maxAnisotropy() const {
  return maxAnisotropy_;
}
