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

#include "yf/cg/Buffer.h"
#include "yf/cg/Encoder.h"

#include "Mesh.h"

SG_NS_BEGIN

/// Types of vertex attribute data (non-interleaved storage).
///
enum VxType {
  VxTypePosition,
  VxTypeTangent,
  VxTypeNormal,
  VxTypeTexCoord0,
  VxTypeTexCoord1,
  VxTypeColor0,
  VxTypeJoints0,
  VxTypeWeights0
};

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

  std::vector<std::unique_ptr<uint8_t[]>> data{};
  std::unordered_map<VxType, Accessor> vxAccessors{};
  Accessor ixAccessor{};

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

  /// Checks whether or not a given vertex type can be bound by this mesh.
  ///
  /// One must not attempt to encode a vertex buffer binding for a type
  /// which `canBind()` returns `false`.
  ///
  bool canBind(VxType type) const;

  /// Checks whether or not this mesh contains an index buffer.
  ///
  /// If `isIndexed()` returns `false`, index buffer binding can be skipped.
  ///
  bool isIndexed() const;

  /// Encodes a vertex buffer binding command for this mesh.
  ///
  void encodeVertexBuffer(CG_NS::GrEncoder& encoder, VxType type,
                          uint32_t inputIndex);

  /// Encodes an index buffer binding command for this mesh.
  ///
  void encodeIndexBuffer(CG_NS::GrEncoder& encoder);

  /// Encodes all required buffer bindings for this mesh.
  ///
  /// Input indices for vertex buffers are taken from the `VxType` enum.
  ///
  void encodeBindings(CG_NS::GrEncoder& encoder);

  /// Encodes a draw command for this mesh.
  ///
  void encodeDraw(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                  uint32_t instanceCount);

  /// Encodes required bindings and a draw command for this mesh.
  ///
  /// This is equivalent to a call to `encodeBindings()` followed by a call
  /// to `encodeDraw()`.
  ///
  void encode(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
              uint32_t instanceCount);

#ifndef YF_DEVEL
 private:
#endif
  /// Range of `buffer_` memory available for use.
  ///
  struct Segment {
    uint64_t offset;
    uint64_t size;
  };

  static CG_NS::Buffer::Ptr buffer_;
  static std::list<Segment> segments_;

  /// Description of data in `buffer_` memory.
  ///
  struct DataEntry {
    uint64_t offset;
    uint32_t count;
    uint32_t stride;
  };

  std::unordered_map<VxType, DataEntry> vxData_{};
  DataEntry ixData_{UINT64_MAX, UINT32_MAX, UINT32_MAX};

  void resizeBuffer(uint64_t);
};

SG_NS_END

#endif // YF_SG_MESHIMPL_H
