//
// yf
// CGEncoder.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_ENCODER_H
#define YF_CG_ENCODER_H

#include <cstdint>
#include <vector>
#include <memory>

#include "YFDefs.h"
#include "CGTypes.h"

YF_NS_BEGIN

enum CGIndexType {
  CGIndexTypeU16,
  CGIndexTypeU32
};

struct CGColor {
  bool operator==(const CGColor& other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
  }
  bool operator!=(const CGColor& other) const {
    return !operator==(other);
  }

  float r;
  float g;
  float b;
  float a;
};

struct CGViewport {
  bool operator==(const CGViewport& other) const {
    return x == other.x && y == other.y &&
           width == other.width && height == other.height &&
           zNear == other.zNear && zFar == other.zFar;
  }
  bool operator!=(const CGViewport& other) const {
    return !operator==(other);
  }

  float x;
  float y;
  float width;
  float height;
  float zNear;
  float zFar;
};

struct CGScissor {
  bool operator==(const CGScissor& other) const {
    return offset == other.offset && size == other.size;
  }
  bool operator!=(const CGScissor& other) const {
    return !operator==(other);
  }

  CGOffset2 offset;
  CGSize2 size;
};

class CGGrState;
class CGCpState;
class CGTarget;
class CGDcTable;
class CGBuffer;

struct CGCmd;
using CGEncoding = std::vector<std::unique_ptr<CGCmd>>;

/// Base encoder.
///
class CGEncoder {
 public:
  enum Type {
    Graphics,
    Compute,
    Transfer
  };

  CGEncoder(Type type);
  virtual ~CGEncoder() = 0;

  Type type() const;
  const CGEncoding& encoding() const;

 protected:
  class Impl;
  std::unique_ptr<Impl> _impl;
};

/// Graphics encoder.
///
class CGGrEncoder final : public CGEncoder {
 public:
  CGGrEncoder();
  ~CGGrEncoder() = default;

  /// Sets the graphics state.
  ///
  void setState(CGGrState* state);

  /// Sets the render area.
  ///
  void setViewport(CGViewport viewport, uint32_t viewportIndex = 0);
  void setScissor(CGScissor scissor, uint32_t viewportIndex = 0);

  /// Sets the render target.
  ///
  void setTarget(CGTarget* target);

  /// Sets a descriptor table allocation.
  ///
  void setDcTable(uint32_t tableIndex, uint32_t allocIndex);

  /// Sets the vertex buffer.
  ///
  void setVertexBuffer(CGBuffer* buffer,
                       uint64_t offset,
                       uint32_t inputIndex = 0);

  /// Sets the index buffer.
  ///
  void setIndexBuffer(CGBuffer* buffer, uint64_t offset, CGIndexType type);

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
  void clearColor(CGColor value, uint32_t colorIndex = 0);
  void clearDepth(float value);
  void clearStencil(uint32_t value);
};

/// Compute encoder.
///
class CGCpEncoder final : public CGEncoder {
 public:
  CGCpEncoder();
  ~CGCpEncoder() = default;

  /// Sets the compute state.
  ///
  void setState(CGCpState* state);

  /// Sets a descriptor table allocation.
  ///
  void setDcTable(uint32_t tableIndex, uint32_t allocIndex);

  /// Dispatches a workgroup.
  ///
  void dispatch(CGSize3 size);
};

/// Transfer encoder.
///
class CGTfEncoder final : public CGEncoder {
 public:
  CGTfEncoder();
  ~CGTfEncoder() = default;

  // TODO
};

YF_NS_END

#endif // YF_CG_ENCODER_H
