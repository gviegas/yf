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

#include "yf/sg/Defs.h"

SG_NS_BEGIN

/// Texture resource.
///
class Texture {
 public:
  using Ptr = std::unique_ptr<Texture>;

  Texture(const std::wstring& pathname);
  Texture(std::ifstream& stream);
  ~Texture();

  size_t hash() const;

  struct Data;
  Texture(const Data& data);

  class Impl;
  Impl& impl();

 private:
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_TEXTURE_H
