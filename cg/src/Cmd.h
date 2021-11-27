//
// CG
// Cmd.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_CMD_H
#define YF_CG_CMD_H

#include "Encoder.h"

CG_NS_BEGIN

/// Base command.
///
struct Cmd {
  /// Identification for command subclasses (no rtti).
  ///
  enum Type {
    ViewportT,
    ScissorT,
    TargetT,
    StateGrT,
    StateCpT,
    DcTableT,
    VxBufferT,
    IxBufferT,
    DrawT,
    DrawIxT,
    DispatchT,
    CopyBBT,
    CopyIIT,
    SyncT
  };

  /// The subclass of this command.
  ///
  const Type cmd;

  explicit Cmd(Type cmd) : cmd(cmd) { }
  virtual ~Cmd() = default;
};

/// Set viewport command.
///
struct ViewportCmd : Cmd {
  Viewport viewport;
  uint32_t viewportIndex;

  ViewportCmd(Viewport viewport, uint32_t viewportIndex)
    : Cmd(ViewportT), viewport(viewport), viewportIndex(viewportIndex) { }
};

/// Set scissor command.
///
struct ScissorCmd : Cmd {
  Scissor scissor;
  uint32_t viewportIndex;

  ScissorCmd(Scissor scissor, uint32_t viewportIndex)
    : Cmd(ScissorT), scissor(scissor), viewportIndex(viewportIndex) { }
};

/// Set render target command.
///
struct TargetCmd : Cmd {
  Target& target;
  TargetOp targetOp;

  TargetCmd(Target& target, const TargetOp& targetOp)
    : Cmd(TargetT), target(target), targetOp(targetOp) { }
};

/// Set state command for graphics.
///
struct StateGrCmd : Cmd {
  GrState& state;

  explicit StateGrCmd(GrState& state) : Cmd(StateGrT), state(state) { }
};

/// Set state command for compute.
///
struct StateCpCmd : Cmd {
  CpState& state;

  explicit StateCpCmd(CpState& state) : Cmd(StateCpT), state(state) { }
};

/// Set descriptor table command.
///
struct DcTableCmd : Cmd {
  uint32_t tableIndex;
  uint32_t allocIndex;

  DcTableCmd(uint32_t tableIndex, uint32_t allocIndex)
    : Cmd(DcTableT), tableIndex(tableIndex), allocIndex(allocIndex) { }
};

/// Set vertex buffer command.
///
struct VxBufferCmd : Cmd {
  Buffer& buffer;
  uint64_t offset;
  uint32_t inputIndex;

  VxBufferCmd(Buffer& buffer, uint64_t offset, uint32_t inputIndex)
    : Cmd(VxBufferT), buffer(buffer), offset(offset), inputIndex(inputIndex) { }
};

/// Set index buffer command.
///
struct IxBufferCmd : Cmd {
  Buffer& buffer;
  uint64_t offset;
  IndexType type;

  IxBufferCmd(Buffer& buffer, uint64_t offset, IndexType type)
    : Cmd(IxBufferT), buffer(buffer), offset(offset), type(type) { }
};

/// Draw command.
///
struct DrawCmd : Cmd {
  uint32_t vertexStart;
  uint32_t vertexCount;
  uint32_t baseInstance;
  uint32_t instanceCount;

  DrawCmd(uint32_t vertexStart, uint32_t vertexCount,
          uint32_t baseInstance, uint32_t instanceCount)
    : Cmd(DrawT), vertexStart(vertexStart), vertexCount(vertexCount),
      baseInstance(baseInstance), instanceCount(instanceCount) { }
};

/// Draw indexed command.
///
struct DrawIxCmd : Cmd {
  uint32_t indexStart;
  uint32_t vertexCount;
  int32_t vertexOffset;
  uint32_t baseInstance;
  uint32_t instanceCount;

  DrawIxCmd(uint32_t indexStart, uint32_t vertexCount, int32_t vertexOffset,
            uint32_t baseInstance, uint32_t instanceCount)
    : Cmd(DrawIxT), indexStart(indexStart), vertexCount(vertexCount),
      vertexOffset(vertexOffset), baseInstance(baseInstance),
      instanceCount(instanceCount) { }
};

/// Dispatch command.
///
struct DispatchCmd : Cmd {
  Size3 size;

  explicit DispatchCmd(Size3 size) : Cmd(DispatchT), size(size) { }
};

/// Copy buffer command.
///
struct CopyBBCmd : Cmd {
  Buffer& dst;
  uint64_t dstOffset;
  Buffer& src;
  uint64_t srcOffset;
  uint64_t size;

  CopyBBCmd(Buffer& dst, uint64_t dstOffset, Buffer& src, uint64_t srcOffset,
            uint64_t size)
    : Cmd(CopyBBT), dst(dst), dstOffset(dstOffset), src(src),
      srcOffset(srcOffset), size(size) { }
};

/// Copy image command.
///
struct CopyIICmd : Cmd {
  Image* dst;
  Offset2 dstOffset;
  uint32_t dstLayer;
  uint32_t dstLevel;
  Image* src;
  Offset2 srcOffset;
  uint32_t srcLayer;
  uint32_t srcLevel;
  Size2 size;
  uint32_t layerCount;

  CopyIICmd(Image* dst, Offset2 dstOffset, uint32_t dstLayer, uint32_t dstLevel,
            Image* src, Offset2 srcOffset, uint32_t srcLayer, uint32_t srcLevel,
            Size2 size, uint32_t layerCount)
    : Cmd(CopyIIT),
      dst(dst), dstOffset(dstOffset), dstLayer(dstLayer), dstLevel(dstLevel),
      src(src), srcOffset(srcOffset), srcLayer(srcLayer), srcLevel(srcLevel),
      size(size), layerCount(layerCount) { }
};

/// Synchronize command.
///
struct SyncCmd : Cmd {
  SyncCmd() : Cmd(SyncT) { }
};

CG_NS_END

#endif // YF_CG_CMD_H
