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

#include "yf/sg/Defs.h"

SG_NS_BEGIN

/// Texture resource.
///
class Texture {
 public:
  /// File types from which a texture can be created.
  ///
  enum FileType {
    Internal,
    Png
  };

  Texture(FileType fileType, const std::wstring& textureFile);
  Texture();
  Texture(const Texture& other);
  Texture& operator=(const Texture& other);
  ~Texture();

  explicit operator bool() const;
  bool operator!() const;
  bool operator==(const Texture& other) const;
  bool operator!=(const Texture& other) const;

  size_t hash() const;

  struct Data;
  Texture(const Data& data);

  class Impl;
  Impl& impl();

 private:
  std::shared_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_TEXTURE_H
