//
// SG
// MeshImpl.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_MESHIMPL_H
#define YF_SG_MESHIMPL_H

#include <cstdint>
#include <vector>

#include "yf/cg/Buffer.h"
#include "yf/cg/Encoder.h"

#include "Mesh.h"

SG_NS_BEGIN

/// Generic mesh data to store in device memory.
///
struct Mesh::Data {
  struct {
    void* data;
    uint64_t size;
  } vertex;
  struct {
    void* data;
    uint32_t count;
    uint32_t stride;
  } index;
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
  void updateVertices(void* data, uint64_t offset, uint64_t size);

  /// Updates index data.
  ///
  void updateIndices(void* data, uint32_t indexStart, uint32_t indexCount);

  /// Encodes vertex/index buffer bindings for this mesh.
  ///
  void encodeBindings(CG_NS::GrEncoder& encoder, uint32_t inputIndex = 0);

  /// Encodes a draw command for this mesh.
  ///
  void encodeDraw(CG_NS::GrEncoder& encoder, uint32_t baseInstance,
                  uint32_t instanceCount);

 private:
  struct Segment {
    uint64_t offset;
    uint64_t size;
  };

  static CG_NS::Buffer::Ptr buffer_;
  static std::vector<Segment> segments_;

  uint64_t vxOffset_ = 0;
  uint64_t vxSize_ = 0;
  uint64_t ixOffset_ = 0;
  uint32_t ixCount_ = 0;
  uint32_t ixStride_ = 0;
};

SG_NS_END

#endif // YF_SG_MESHIMPL_H
