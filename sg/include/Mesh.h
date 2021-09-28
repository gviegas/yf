//
// SG
// Mesh.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_SG_MESH_H
#define YF_SG_MESH_H

#include <cstddef>
#include <memory>
#include <string>
#include <fstream>

#include "yf/sg/Defs.h"

SG_NS_BEGIN

/// Mesh resource.
///
class Mesh {
 public:
  using Ptr = std::unique_ptr<Mesh>;

  Mesh(const std::wstring& pathname, size_t index = 0);
  Mesh(std::ifstream& stream, size_t index = 0);
  ~Mesh();

  size_t hash() const;

  struct Data;
  Mesh(const Data& data);

  class Impl;
  Impl& impl();

 private:
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_MESH_H
