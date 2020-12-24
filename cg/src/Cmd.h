//
// CG
// Cmd.h
//
// Copyright © 2020 Gustavo C. Viegas.
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
    StateGrT,
    StateCpT,
    ViewportT,
    ScissorT,
    TargetT,
    DcTableT,
    VxBufferT,
    IxBufferT,
    DrawT,
    DrawIxT,
    DispatchT,
    ClearClT,
    ClearDpT,
    ClearScT
  };

  explicit Cmd(Type cmd) : cmd(cmd) { }
  virtual ~Cmd() = default;

  /// The subclass of this command.
  ///
  const Type cmd;
};

/// Set state command for graphics.
///
struct StateGrCmd : Cmd {
  explicit StateGrCmd(GrState* state) : Cmd(StateGrT), state(state) { }

  GrState* state;
};

/// Set state command for compute.
///
struct StateCpCmd : Cmd {
  explicit StateCpCmd(CpState* state) : Cmd(StateCpT), state(state) { }

  CpState* state;
};

/// Set viewport command.
///
struct ViewportCmd : Cmd {
  ViewportCmd(Viewport viewport, uint32_t viewportIndex)
    : Cmd(ViewportT), viewport(viewport), viewportIndex(viewportIndex) { }

  Viewport viewport;
  uint32_t viewportIndex;
};

/// Set scissor command.
///
struct ScissorCmd : Cmd {
  explicit ScissorCmd(Scissor scissor, uint32_t viewportIndex)
    : Cmd(ScissorT), scissor(scissor), viewportIndex(viewportIndex) { }

  Scissor scissor;
  uint32_t viewportIndex;
};

/// Set render target command.
///
struct TargetCmd : Cmd {
  explicit TargetCmd(Target* target) : Cmd(TargetT), target(target) { }

  Target* target;
};

/// Set descriptor table command.
///
struct DcTableCmd : Cmd {
  DcTableCmd(uint32_t tableIndex, uint32_t allocIndex)
    : Cmd(DcTableT), tableIndex(tableIndex), allocIndex(allocIndex) { }

  uint32_t tableIndex;
  uint32_t allocIndex;
};

/// Set vertex buffer command.
///
struct VxBufferCmd : Cmd {
  VxBufferCmd(Buffer* buffer, uint64_t offset, uint32_t inputIndex)
    : Cmd(VxBufferT), buffer(buffer), offset(offset), inputIndex(inputIndex) { }

  Buffer* buffer;
  uint64_t offset;
  uint32_t inputIndex;
};

/// Set index buffer command.
///
struct IxBufferCmd : Cmd {
  IxBufferCmd(Buffer* buffer, uint64_t offset, IndexType type)
    : Cmd(IxBufferT), buffer(buffer), offset(offset), type(type) { }

  Buffer* buffer;
  uint64_t offset;
  IndexType type;
};

/// Draw command.
///
struct DrawCmd : Cmd {
  DrawCmd(uint32_t vertexStart, uint32_t vertexCount,
          uint32_t baseInstance, uint32_t instanceCount)
    : Cmd(DrawT), vertexStart(vertexStart), vertexCount(vertexCount),
      baseInstance(baseInstance), instanceCount(instanceCount) { }

  uint32_t vertexStart;
  uint32_t vertexCount;
  uint32_t baseInstance;
  uint32_t instanceCount;
};

/// Draw indexed command.
///
struct DrawIxCmd : Cmd {
  DrawIxCmd(uint32_t indexStart, uint32_t vertexCount, int32_t vertexOffset,
            uint32_t baseInstance, uint32_t instanceCount)
    : Cmd(DrawIxT), indexStart(indexStart), vertexCount(vertexCount),
      vertexOffset(vertexOffset), baseInstance(baseInstance),
      instanceCount(instanceCount) { }

  uint32_t indexStart;
  uint32_t vertexCount;
  int32_t vertexOffset;
  uint32_t baseInstance;
  uint32_t instanceCount;
};

/// Dispatch command.
///
struct DispatchCmd : Cmd {
  explicit DispatchCmd(Size3 size) : Cmd(DispatchT), size(size) { }

  Size3 size;
};

/// Clear color command.
///
struct ClearClCmd : Cmd {
  ClearClCmd(Color value, uint32_t colorIndex)
    : Cmd(ClearClT), value(value), colorIndex(colorIndex) { }

  Color value;
  uint32_t colorIndex;
};

/// Clear depth command.
///
struct ClearDpCmd : Cmd {
  explicit ClearDpCmd(float value) : Cmd(ClearDpT), value(value) { }

  float value;
};

/// Clear stencil command.
///
struct ClearScCmd : Cmd {
  explicit ClearScCmd(uint32_t value) : Cmd(ClearScT), value(value) { }

  uint32_t value;
};

CG_NS_END

#endif // YF_CG_CMD_H
