//
// yf
// CGEncoder.h
//
// Copyright (c) 2020 Gustavo C. Viegas.
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
  float r;
  float g;
  float b;
  float a;
};

struct CGViewport {
  float x;
  float y;
  float width;
  float height;
  float zNear;
  float zFar;
};

struct CGScissor {
  CGOffset2 offset;
  CGSize2 size;
};

class CGGraphState;
class CGCompState;
class CGTarget;
class CGDcTable;
class CGBuffer;

struct CGCmd;
using CGEncoding = std::vector<std::unique_ptr<CGCmd>>;

class CGEncoder {
 public:
  CGEncoder();
  ~CGEncoder();

  /// Sets the state for graphics operations.
  ///
  void setState(CGGraphState* state);

  /// Sets the state for compute operations.
  ///
  void setState(CGCompState* state);

  /// Sets the render area.
  ///
  void setViewport(CGViewport viewport);
  void setScissor(CGScissor scissor);

  /// Sets the render target.
  ///
  void setTarget(CGTarget* target);

  /// Sets a descriptor table allocation.
  ///
  void setDcTable(uint32_t tableIndex, uint32_t allocIndex);

  /// Sets the vertex buffer.
  ///
  void setVertexBuffer(uint32_t inputIndex, CGBuffer* buffer, uint64_t offset);

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

  /// Dispatches a workgroup.
  ///
  void dispatch(CGSize3 size);

  /// Clears the bound target.
  ///
  void clearColor(uint32_t colorIndex, CGColor value);
  void clearDepth(float value);
  void clearStencil(uint32_t value);

  // TODO:
  // - copy buf/img
  // - clear buf/img
  // - sync
  // - ...

  const CGEncoding& encoding() const;

 private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};

YF_NS_END

#endif // YF_CG_ENCODER_H
