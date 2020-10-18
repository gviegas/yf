//
// cg
// Encoder.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_ENCODER_H
#define YF_CG_ENCODER_H

#include <cstdint>
#include <vector>
#include <memory>

#include "yf/cg/Defs.h"
#include "yf/cg/Types.h"

CG_NS_BEGIN

/// Type of index data in an index buffer.
///
enum IndexType {
  IndexTypeU16,
  IndexTypeU32
};

/// Normalized RGBA color.
///
struct Color {
  bool operator==(const Color& other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
  }
  bool operator!=(const Color& other) const {
    return !operator==(other);
  }

  float r;
  float g;
  float b;
  float a;
};

/// Viewport.
///
struct Viewport {
  bool operator==(const Viewport& other) const {
    return x == other.x && y == other.y &&
           width == other.width && height == other.height &&
           zNear == other.zNear && zFar == other.zFar;
  }
  bool operator!=(const Viewport& other) const {
    return !operator==(other);
  }

  float x;
  float y;
  float width;
  float height;
  float zNear;
  float zFar;
};

/// Scissor.
///
struct Scissor {
  bool operator==(const Scissor& other) const {
    return offset == other.offset && size == other.size;
  }
  bool operator!=(const Scissor& other) const {
    return !operator==(other);
  }

  Offset2 offset;
  Size2 size;
};

class GrState;
class CpState;
class Target;
class DcTable;
class Buffer;

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
  std::unique_ptr<Impl> _impl;
};

/// Graphics encoder.
///
class GrEncoder final : public Encoder {
 public:
  GrEncoder();
  ~GrEncoder() = default;

  /// Sets the graphics state.
  ///
  void setState(GrState* state);

  /// Sets the render area.
  ///
  void setViewport(Viewport viewport, uint32_t viewportIndex = 0);
  void setScissor(Scissor scissor, uint32_t viewportIndex = 0);

  /// Sets the render target.
  ///
  void setTarget(Target* target);

  /// Sets a descriptor table allocation.
  ///
  void setDcTable(uint32_t tableIndex, uint32_t allocIndex);

  /// Sets the vertex buffer.
  ///
  void setVertexBuffer(Buffer* buffer,
                       uint64_t offset,
                       uint32_t inputIndex = 0);

  /// Sets the index buffer.
  ///
  void setIndexBuffer(Buffer* buffer, uint64_t offset, IndexType type);

  /// Draws primitives.
  ///
  void draw(uint32_t vertexStart,
            uint32_t vertexCount,
            uint32_t baseInstance = 0,
            uint32_t instanceCount = 1);

  /// Draws primitives, indexed.
  ///
  void drawIndexed(uint32_t indexStart,
                   uint32_t vertexCount,
                   int32_t vertexOffset = 0,
                   uint32_t baseInstance = 0,
                   uint32_t instanceCount = 1);

  /// Clears the bound target.
  ///
  void clearColor(Color value, uint32_t colorIndex = 0);
  void clearDepth(float value);
  void clearStencil(uint32_t value);
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
};

/// Transfer encoder.
///
class TfEncoder final : public Encoder {
 public:
  TfEncoder();
  ~TfEncoder() = default;

  // TODO
};

CG_NS_END

#endif // YF_CG_ENCODER_H
