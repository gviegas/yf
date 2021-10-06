//
// SG
// MeshImpl.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_SG_MESHIMPL_H
#define YF_SG_MESHIMPL_H

#include <cstdint>
#include <list>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include "yf/cg/Buffer.h"
#include "yf/cg/State.h"
#include "yf/cg/Encoder.h"

#include "Mesh.h"

#ifdef YF_DEVEL
# include "../test/Test.h"
TEST_NS_BEGIN struct MeshTest; TEST_NS_END
#endif

SG_NS_BEGIN

/// Types of vertex attribute data (non-interleaved storage).
///
enum VxType {
  VxTypePosition  = 0,
  VxTypeTangent   = 1,
  VxTypeNormal    = 2,
  VxTypeTexCoord0 = 3,
  VxTypeTexCoord1 = 4,
  VxTypeColor0    = 5,
  VxTypeJoints0   = 6,
  VxTypeWeights0  = 7
};

/// Produces a vertex input object for a given vertex attribute type.
///
/// `Renderer` creates graphics states using these inputs, thus mesh data
/// must be provided accordingly.
///
inline CG_NS::VxInput vxInputFor(VxType type) {
  CG_NS::VxFormat format;
  uint32_t stride;

  switch (type) {
  case VxTypePosition:
  case VxTypeNormal:
    format = CG_NS::VxFormatFlt3;
    stride = sizeof(float[3]);
    break;
  case VxTypeTangent:
  case VxTypeColor0:
  case VxTypeWeights0:
    format = CG_NS::VxFormatFlt4;
    stride = sizeof(float[4]);
    break;
  case VxTypeTexCoord0:
  case VxTypeTexCoord1:
    format = CG_NS::VxFormatFlt2;
    stride = sizeof(float[2]);
    break;
  case VxTypeJoints0:
    format = CG_NS::VxFormatUbyte4;
    stride = 4;
    break;
  default:
    throw std::invalid_argument("Invalid VxType value");
  }

  return {{{type, format, 0}}, stride, CG_NS::VxStepFnVertex};
}

/// Generic mesh data for copying.
///
struct Mesh::Data {
  /// Data accessor.
  ///
  /// Mesh data can be provided in any number of (CPU) `data` buffers.
  /// The `Accessor` describes the location and layout of a specific data type.
  ///
  struct Accessor {
    uint32_t dataIndex = UINT32_MAX;
    uint64_t dataOffset = UINT64_MAX;
    uint32_t elementN = UINT32_MAX;
    uint32_t elementSize = UINT32_MAX;
  };

  /// Primitive.
  ///
  /// Mesh data defines one or more primitives for rendering.
  /// Each `Primitive` corresponds to a separate drawing command.
  ///
  struct Primitive {
    CG_NS::Topology topology = CG_NS::TopologyTriangle;
    std::unordered_map<VxType, Accessor> vxAccessors{};
    Accessor ixAccessor{};
  };

  std::vector<std::unique_ptr<char[]>> data{};
  std::vector<Primitive> primitives{};

  Data() = default;
  Data(const Data&) = delete;
  Data& operator=(const Data&) = delete;
  ~Data() = default;
};

/// Mesh implementation details.
///
class Mesh::Impl {
 public:
  Impl(const Data& data);
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  ~Impl();

  /// Gets the number of primitives in the mesh.
  ///
  uint32_t primitiveCount() const;

  /// Gets the topology of a mesh primitive.
  ///
  CG_NS::Topology topology(uint32_t primitive) const;

  /// Checks whether or not a given vertex type can be bound by the mesh.
  ///
  /// One must not attempt to encode a vertex buffer binding for a type
  /// which `canBind()` returns `false`.
  ///
  bool canBind(VxType type, uint32_t primitive) const;
  bool canBind(VxType type) const;

  /// Checks whether or not the mesh contains an index buffer.
  ///
  /// If `isIndexed()` returns `false`, index buffer binding can be skipped.
  ///
  bool isIndexed(uint32_t primitive) const;
  bool isIndexed() const;

  /// Encodes a vertex buffer binding command.
  ///
  void encodeVertexBuffer(CG_NS::GrEncoder& encoder, uint32_t inputIndex,
                          VxType type, uint32_t primitive);

  /// Encodes an index buffer binding command.
  ///
  void encodeIndexBuffer(CG_NS::GrEncoder& encoder, uint32_t primitive);

  /// Encodes all required buffer bindings.
  ///
  /// Input indices for vertex buffers are taken from the `VxType` enum.
  ///
  void encodeBindings(CG_NS::GrEncoder& encoder, uint32_t primitive);

  /// Encodes a draw command.
  ///
  void encodeDraw(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                  uint32_t instanceCount, uint32_t primitive);

  /// Encodes required bindings and draw command(s).
  ///
  /// This is equivalent to a call to `encodeBindings()` followed by a call
  /// to `encodeDraw()`.
  ///
  void encode(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
              uint32_t instanceCount, uint32_t primitive);

  void encode(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
              uint32_t instanceCount);

 private:
  /// Range of `buffer_` memory available for use.
  ///
  struct Segment {
    uint64_t offset = UINT64_MAX;
    uint64_t size = UINT64_MAX;
  };

  static CG_NS::Buffer::Ptr buffer_;
  static std::list<Segment> segments_;

  /// Description of data in `buffer_` memory.
  ///
  struct DataEntry {
    uint64_t offset = UINT64_MAX;
    uint32_t count = UINT32_MAX;
    uint32_t stride = UINT32_MAX;
  };

  /// Mesh primitive.
  ///
  struct Primitive {
    CG_NS::Topology topology = CG_NS::TopologyTriangle;
    std::unordered_map<VxType, DataEntry> vxData{};
    DataEntry ixData{};
  };

  std::vector<Primitive> primitives_{};

  bool resizeBuffer(uint64_t);

#ifdef YF_DEVEL
  friend TEST_NS::MeshTest;
#endif
};

SG_NS_END

#endif // YF_SG_MESHIMPL_H
