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

#include "yf/sg/Defs.h"

SG_NS_BEGIN

/// Mesh resource.
///
class Mesh {
 public:
  /// File types from which a mesh can be created.
  ///
  enum FileType {
    Internal,
    Gltf,
    Obj
  };

  Mesh(FileType fileType, const std::wstring& meshFile, size_t index = 0);
  Mesh();
  Mesh(const Mesh& other);
  Mesh& operator=(const Mesh& other);
  ~Mesh();

  explicit operator bool() const;
  bool operator!() const;
  bool operator==(const Mesh& other) const;
  bool operator!=(const Mesh& other) const;

  size_t hash() const;

  struct Data;
  Mesh(const Data& data);

  class Impl;
  Impl& impl();

 private:
  std::shared_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_MESH_H
