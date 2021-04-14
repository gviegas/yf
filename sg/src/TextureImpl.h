//
// SG
// TextureImpl.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_SG_TEXTUREIMPL_H
#define YF_SG_TEXTUREIMPL_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>

#include "yf/cg/Image.h"
#include "yf/cg/DcTable.h"

#include "Texture.h"

SG_NS_BEGIN

/// Generic texture data for copying.
///
struct Texture::Data {
  std::unique_ptr<uint8_t[]> data;
  CG_NS::PxFormat format;
  CG_NS::Size2 size;
  uint32_t levels;
  CG_NS::Samples samples;

  explicit Data(uint8_t* data = nullptr,
                CG_NS::PxFormat format = CG_NS::PxFormatUndefined,
                CG_NS::Size2 size = {0}, uint32_t levels = 1,
                CG_NS::Samples samples = CG_NS::Samples1)
    : data(data), format(format), size(size), levels(levels),
      samples(samples) { }

  Data(const Data&) = delete;
  Data& operator=(const Data&) = delete;
  ~Data() = default;
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
  void copy(CG_NS::DcTable& dcTable, uint32_t allocation, CG_NS::DcId id,
            uint32_t element, uint32_t level, CG_NS::Sampler* sampler);

#ifndef YF_DEVEL
 private:
#endif
  /// Key for the resource map.
  ///
  struct Key {
    CG_NS::PxFormat format;
    CG_NS::Size2 size;
    uint32_t levels;
    CG_NS::Samples samples;

    explicit Key(CG_NS::PxFormat format = CG_NS::PxFormatUndefined,
                 CG_NS::Size2 size = {0}, uint32_t levels = 1,
                 CG_NS::Samples samples = CG_NS::Samples1)
      : format(format), size(size), levels(levels), samples(samples) { }

    bool operator==(const Key& other) const {
      return format == other.format && size == other.size &&
             levels == other.levels && samples == other.samples;
    }

    struct Hash {
      size_t operator()(const Key& key) const {
        const size_t f = key.format;
        const size_t w = key.size.width;
        const size_t h = key.size.height;
        const size_t l = key.levels;
        const size_t s = key.samples;
        return (f << 16) ^ ((w << 16) | h) ^ (l << 24) ^ (s << 8) ^ 0xDD1698C9;
      }
    };
  };

  /// Image resource.
  ///
  struct Resource {
    CG_NS::Image::Ptr image;
    struct {
      std::vector<bool> unused;
      uint32_t remaining;
      uint32_t current;
    } layers;
  };

  using Resources = std::unordered_map<Key, Resource, Key::Hash>;
  static Resources resources_;

  Key key_{};
  uint32_t layer_ = UINT32_MAX;

  bool setLayerCount(Resource&, uint32_t);
};

SG_NS_END

#endif // YF_SG_TEXTUREIMPL_H
