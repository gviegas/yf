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

#include "yf/cg/State.h"

#include "yf/sg/Defs.h"
#include "yf/sg/Material.h"

SG_NS_BEGIN

/// Vertex data semantics.
///
enum VxData : uint32_t {
  // Vertex attributes
  VxDataPosition  = 0x01,
  VxDataNormal    = 0x02,
  VxDataTangent   = 0x04,
  VxDataTexCoord0 = 0x08,
  VxDataTexCoord1 = 0x10,
  VxDataColor0    = 0x20,
  VxDataJoints0   = 0x40,
  VxDataWeights0  = 0x80,

  // Vertex indices
  VxDataIndices = 0x100,

  VxDataUndefined = 0
};

using VxDataMask = uint32_t;

/// Primitive.
///
class Primitive {
 public:
  using Ptr = std::unique_ptr<Primitive>;

  Primitive();
  ~Primitive();

  /// Primitive's topology.
  ///
  CG_NS::Topology topology() const;

  /// `VxData` mask describing the Primitive's data.
  ///
  VxDataMask dataMask() const;

  /// Material properties.
  ///
  const Material* material() const;

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

  /// Mesh data for direct initialization.
  ///
  struct Data {
    /// Data accessor.
    ///
    /// Mesh data can be provided in any number of (CPU) `data` buffers.
    /// The `Accessor` describes the semantic, location and layout of a
    /// specific data type.
    ///
    struct Accessor {
      VxData semantic = VxDataUndefined;
      uint32_t dataIndex = UINT32_MAX;
      uint64_t dataOffset = UINT64_MAX;
      uint32_t elementN = UINT32_MAX;
      uint32_t elementSize = UINT32_MAX;
    };

    /// Primitive data.
    ///
    /// Mesh data defines one or more primitives for rendering.
    /// Each `Primitive` corresponds to a separate draw command.
    ///
    struct Primitive {
      CG_NS::Topology topology = CG_NS::TopologyTriangle;
      std::vector<Accessor> accessors{};
      Material::Ptr material{};
    };

    std::vector<std::unique_ptr<char[]>> data{};
    std::vector<Primitive> primitives{};

    Data() = default;
    Data(const Data&) = delete;
    Data& operator=(const Data&) = delete;
    ~Data() = default;
  };

  Mesh(const Data& data);
  ~Mesh();

  /// Gets a given primitive.
  ///
  Primitive& primitive(uint32_t index);
  const Primitive& primitive(uint32_t index) const;
  Primitive& operator[](uint32_t index);
  const Primitive& operator[](uint32_t index) const;

  /// Gets the number of primitives in the mesh.
  ///
  uint32_t primitiveCount() const;

  size_t hash() const;

  class Impl;
  Impl& impl();

 private:
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_MESH_H
