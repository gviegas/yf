//
// CG
// Encoder.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_ENCODER_H
#define YF_CG_ENCODER_H

#include <cstdint>
#include <memory>
#include <array>
#include <vector>
#include <utility>

#include "yf/cg/Defs.h"
#include "yf/cg/Types.h"

CG_NS_BEGIN

/// Viewport.
///
struct Viewport {
  float x;
  float y;
  float width;
  float height;
  float zNear;
  float zFar;

  bool operator==(const Viewport& other) const {
    return x == other.x && y == other.y &&
           width == other.width && height == other.height &&
           zNear == other.zNear && zFar == other.zFar;
  }

  bool operator!=(const Viewport& other) const {
    return !operator==(other);
  }
};

/// Scissor.
///
struct Scissor {
  Offset2 offset;
  Size2 size;

  bool operator==(const Scissor& other) const {
    return offset == other.offset && size == other.size;
  }

  bool operator!=(const Scissor& other) const {
    return !operator==(other);
  }
};

/// Load operations for attachments.
///
enum LoadOp {
  LoadOpClear,
  LoadOpLoad,
  LoadOpDontCare
};

/// Store operations for attachments.
///
enum StoreOp {
  StoreOpStore,
  StoreOpDontCare
};

/// Load/Store operations of a given attachment.
///
using LoadStoreOp = std::pair<LoadOp, StoreOp>;

/// Normalized RGBA color.
///
using Color = std::array<float, 4>;

/// Set of operations for a render target.
///
struct TargetOp {
  std::vector<LoadStoreOp> colorOps;
  std::vector<Color> colorValues;
  LoadStoreOp depthOp;
  float depthValue;
  LoadStoreOp stencilOp;
  uint32_t stencilValue;
};

/// Type of index data in an index buffer.
///
enum IndexType {
  IndexTypeU16,
  IndexTypeU32
};

class GrState;
class CpState;
class Target;
class DcTable;
class Buffer;
class Image;

struct Cmd;
using Encoding = std::vector<std::unique_ptr<Cmd>>;

/// Base encoder.
///
class Encoder {
 public:
  enum Type {
    Graphics,
    Compute,
    Transfer
  };

  Encoder(Type type);
  virtual ~Encoder() = 0;

  Type type() const;
  const Encoding& encoding() const;

 protected:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

/// Graphics encoder.
///
class GrEncoder final : public Encoder {
 public:
  GrEncoder();
  ~GrEncoder() = default;

  /// Sets the render area.
  ///
  void setViewport(Viewport viewport, uint32_t viewportIndex = 0);
  void setScissor(Scissor scissor, uint32_t viewportIndex = 0);

  /// Sets the render target.
  ///
  void setTarget(Target& target, const TargetOp& targetOp);

  /// Sets the graphics state.
  ///
  void setState(GrState* state);

  /// Sets a descriptor table allocation.
  ///
  void setDcTable(uint32_t tableIndex, uint32_t allocIndex);

  /// Sets the vertex buffer.
  ///
  void setVertexBuffer(Buffer* buffer, uint64_t offset, uint32_t inputIndex);

  /// Sets the index buffer.
  ///
  void setIndexBuffer(Buffer* buffer, uint64_t offset, IndexType type);

  /// Draws primitives.
  ///
  void draw(uint32_t vertexStart, uint32_t vertexCount,
            uint32_t baseInstance = 0, uint32_t instanceCount = 1);

  /// Draws primitives, indexed.
  ///
  void drawIndexed(uint32_t indexStart, uint32_t vertexCount,
                   int32_t vertexOffset = 0, uint32_t baseInstance = 0,
                   uint32_t instanceCount = 1);

  /// Synchronizes commands.
  ///
  void synchronize();
};

/// Compute encoder.
///
class CpEncoder final : public Encoder {
 public:
  CpEncoder();
  ~CpEncoder() = default;

  /// Sets the compute state.
  ///
  void setState(CpState* state);

  /// Sets a descriptor table allocation.
  ///
  void setDcTable(uint32_t tableIndex, uint32_t allocIndex);

  /// Dispatches a workgroup.
  ///
  void dispatch(Size3 size);

  /// Synchronizes commands.
  ///
  void synchronize();
};

/// Transfer encoder.
///
class TfEncoder final : public Encoder {
 public:
  TfEncoder();
  ~TfEncoder() = default;

  /// Copies data between buffers.
  ///
  void copy(Buffer* dst, uint64_t dstOffset, Buffer* src, uint64_t srcOffset,
            uint64_t size);

  /// Copies data between images.
  ///
  void copy(Image* dst, Offset2 dstOffset, uint32_t dstLayer, uint32_t dstLevel,
            Image* src, Offset2 srcOffset, uint32_t srcLayer, uint32_t srcLevel,
            Size2 size, uint32_t layerCount);
};

CG_NS_END

#endif // YF_CG_ENCODER_H
