//
// SG
// TextureImpl.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_TEXTUREIMPL_H
#define YF_SG_TEXTUREIMPL_H

#include <cstdint>
#include <unordered_map>

#include "yf/cg/Image.h"
#include "yf/cg/DcTable.h"

#include "Texture.h"

SG_NS_BEGIN

/// Generic texture data for copying.
///
struct Texture::Data {
  explicit Data(const void* data = nullptr,
                CG_NS::PxFormat format = CG_NS::PxFormatUndefined,
                CG_NS::Size2 size = {0}, uint32_t levels = 1,
                CG_NS::Samples samples = CG_NS::Samples1)
    : data(data), format(format), size(size), levels(levels),
      samples(samples) { }

  const void* data;
  CG_NS::PxFormat format;
  CG_NS::Size2 size;
  uint32_t levels;
  CG_NS::Samples samples;
};

/// Texture implementation details.
///
class Texture::Impl {
 public:
  Impl(const Data& data);
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  ~Impl();

  /// Updates image data.
  ///
  void updateImage(CG_NS::Offset2 offset, CG_NS::Size2 size, uint32_t level,
                   const void* data);

  /// Copies image data to a descriptor table.
  ///
  void copy(CG_NS::DcTable* dcTable, uint32_t allocation, CG_NS::DcId id,
            uint32_t element, uint32_t level, CG_NS::ImgSampler sampler);

 private:
  // TODO
};

SG_NS_END

#endif // YF_SG_TEXTUREIMPL_H
