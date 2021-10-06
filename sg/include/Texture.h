//
// SG
// Texture.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_SG_TEXTURE_H
#define YF_SG_TEXTURE_H

#include <cstddef>
#include <memory>
#include <string>
#include <fstream>

#include "yf/cg/Image.h"

#include "yf/sg/Defs.h"

SG_NS_BEGIN

/// Texture coordinate sets.
///
enum TexCoordSet {
  TexCoordSet0,
  TexCoordSet1
};

/// Texture resource.
///
class Texture {
 public:
  using Ptr = std::unique_ptr<Texture>;

  Texture(const std::string& pathname);
  Texture(std::ifstream& stream);

  /// Texture data for direct initialization.
  ///
  struct Data {
    std::unique_ptr<char[]> data{};
    CG_NS::PxFormat format = CG_NS::PxFormatUndefined;
    CG_NS::Size2 size{0};
    uint32_t levels = 1;
    CG_NS::Samples samples = CG_NS::Samples1;
    CG_NS::Sampler sampler{};
    TexCoordSet coordSet = TexCoordSet0;

    Data() = default;
    Data(const Data&) = delete;
    Data& operator=(const Data&) = delete;
    ~Data() = default;
  };

  Texture(const Data& data);
  ~Texture();

  size_t hash() const;

  class Impl;
  Impl& impl();

 private:
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_TEXTURE_H
