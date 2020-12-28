//
// SG
// MeshImpl.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_MESHIMPL_H
#define YF_SG_MESHIMPL_H

#include <cstdint>
#include <list>
#include <memory>

#include "yf/cg/Buffer.h"
#include "yf/cg/Encoder.h"

#include "Mesh.h"

SG_NS_BEGIN

/// Generic mesh data for copying.
///
struct Mesh::Data {
  explicit Data(uint8_t* vxData = nullptr, uint32_t vxCount = 0,
                uint32_t vxStride = 0, uint8_t* ixData = nullptr,
                uint32_t ixCount = 0, uint32_t ixStride = 0)
    : vxData(vxData), vxCount(vxCount), vxStride(vxStride), ixData(ixData),
      ixCount(ixCount), ixStride(ixStride) { }

  Data(const Data&) = delete;
  Data& operator=(const Data&) = delete;
  ~Data() = default;

  std::unique_ptr<uint8_t[]> vxData;
  uint32_t vxCount;
  uint32_t vxStride;
  std::unique_ptr<uint8_t[]> ixData;
  uint32_t ixCount;
  uint32_t ixStride;
};

/// Mesh implementation details.
///
class Mesh::Impl {
 public:
  Impl(const Data& data);
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  ~Impl();

  /// Updates vertex data.
  ///
  void updateVertices(uint32_t vertexStart, uint32_t vertexCount,
                      const void* data);

  /// Updates index data.
  ///
  void updateIndices(uint32_t indexStart, uint32_t indexCount,
                     const void* data);

  /// Encodes vertex/index buffer bindings for this mesh.
  ///
  void encodeBindings(CG_NS::GrEncoder& encoder, uint32_t inputIndex = 0);

  /// Encodes a draw command for this mesh.
  ///
  void encodeDraw(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                  uint32_t instanceCount);

#ifndef YF_DEVEL
 private:
#endif
  struct Segment {
    uint64_t offset;
    uint64_t size;
  };

  static CG_NS::Buffer::Ptr buffer_;
  static std::list<Segment> segments_;

  uint64_t vxOffset_ = UINT64_MAX;
  uint32_t vxCount_ = 0;
  uint32_t vxStride_ = 0;
  uint64_t ixOffset_ = UINT64_MAX;
  uint32_t ixCount_ = 0;
  uint32_t ixStride_ = 0;
};

SG_NS_END

#endif // YF_SG_MESHIMPL_H
