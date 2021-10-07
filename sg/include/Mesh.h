//
// SG
// Mesh.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_SG_MESH_H
#define YF_SG_MESH_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <fstream>

#include "yf/sg/Defs.h"

SG_NS_BEGIN

/// Vertex data semantics.
///
enum VxData : uint32_t {
  // Vextex attributes
  VxDataPosition  = 0x01,
  VxDataTangent   = 0x02,
  VxDataNormal    = 0x04,
  VxDataTexCoord0 = 0x08,
  VxDataTexCoord1 = 0x10,
  VxDataColor0    = 0x20,
  VxDataJoints0   = 0x40,
  VxDataWeights0  = 0x80,

  // Vertex indices
  VxDataIndices = 0x100,

  VxDataUndefined = 0
};

/// Primitive.
///
class Primitive {
 public:
  using Ptr = std::unique_ptr<Primitive>;

  struct Data;
  Primitive(const Data& data);
  ~Primitive();

  class Impl;
  Impl& impl();

 private:
  std::unique_ptr<Impl> impl_;
};

/// Mesh resource.
///
class Mesh {
 public:
  using Ptr = std::unique_ptr<Mesh>;

  Mesh(const std::string& pathname, size_t index = 0);
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
