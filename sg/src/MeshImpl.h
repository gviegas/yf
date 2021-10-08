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

/// Produces the vertex input binding of a given semantic.
///
inline uint32_t vxInputIndexFor(VxData semantic) {
  uint32_t bit = semantic, index = 0;
  while ((bit >>= 1) != 0)
    index++;
  return index;
}

/// Produces the vertex ID of a given semantic.
///
inline CG_NS::VxId vxIdFor(VxData semantic) {
  return vxInputIndexFor(semantic);
}

/// Produces the vertex input object of a given semantic.
///
/// `Renderer` creates graphics states using these inputs, thus mesh data
/// must be provided accordingly.
///
inline CG_NS::VxInput vxInputFor(VxData semantic) {
  CG_NS::VxFormat format;
  uint32_t stride;

  switch (semantic) {
  case VxDataPosition:
  case VxDataNormal:
    format = CG_NS::VxFormatFlt3;
    stride = sizeof(float[3]);
    break;
  case VxDataTangent:
  case VxDataColor0:
  case VxDataWeights0:
    format = CG_NS::VxFormatFlt4;
    stride = sizeof(float[4]);
    break;
  case VxDataTexCoord0:
  case VxDataTexCoord1:
    format = CG_NS::VxFormatFlt2;
    stride = sizeof(float[2]);
    break;
  case VxDataJoints0:
    format = CG_NS::VxFormatUbyte4;
    stride = 4;
    break;
  default:
    throw std::invalid_argument("Invalid VxData value");
  }

  return {{{vxIdFor(semantic), format, 0}}, stride, CG_NS::VxStepFnVertex};
}

/// Primitive implementation details.
///
class Primitive::Impl {
 public:
  Impl() = default;
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  ~Impl();

  /// Getters.
  ///
  CG_NS::Topology& topology();
  CG_NS::Topology topology() const;
  VxDataMask dataMask() const;

  /// Sets primitive data.
  ///
  void setData(VxData semantic, uint32_t elementN, uint32_t elementSize,
               const void* data);

  /// Encodes vertex/index buffer bindings.
  ///
  void encodeBindings(CG_NS::GrEncoder& encoder);

  /// Encodes draw command.
  ///
  void encodeDraw(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
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

  CG_NS::Topology topology_ = CG_NS::TopologyTriangle;
  uint32_t vxCount_ = 0;
  std::vector<std::pair<VxData, DataEntry>> attributes_{};
  DataEntry indices_{};
  VxDataMask dataMask_ = 0;

  void yieldEntry(const DataEntry&);
  bool resizeBuffer(uint64_t);
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
