//
// SG
// Mesh.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_MESH_H
#define YF_SG_MESH_H

#include <string>
#include <memory>

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
    Collada,
    Obj
  };

  Mesh(FileType fileType, const std::wstring& meshFile);
  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;
  ~Mesh();

  struct Data;
  Mesh(const Data& data);

  class Impl;
  Impl& impl();

 private:
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_MESH_H
