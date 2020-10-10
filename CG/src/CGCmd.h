//
// yf
// CGCmd.h
//
// Copyright (c) 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_CMD_H
#define YF_CG_CMD_H

#include "CGEncoder.h"

YF_NS_BEGIN

/// Base command.
///
struct CGCmd {
  enum Cmd {
    StateGp,
    StateCm,
    Viewport,
    Scissor,
    Target,
    DcTable,
    VxBuffer,
    IxBuffer,
    Draw,
    DrawIx,
    Dispatch,
    ClearCl,
    ClearDp,
    ClearSc
  };

  explicit CGCmd(Cmd cmd) : cmd(cmd) {}
  virtual ~CGCmd() = default;

  const Cmd cmd;
};

/// Set state command for graphics.
///
struct CGStateGpCmd : CGCmd {
  explicit CGStateGpCmd(CGGraphState* state)
    : CGCmd(StateGp), state(state) {}

  CGGraphState* state;
};

/// Set state command for compute.
///
struct CGStateCmCmd : CGCmd {
  explicit CGStateCmCmd(CGCompState* state)
    : CGCmd(StateCm), state(state) {}

  CGCompState* state;
};

/// Set viewport command.
///
struct CGViewportCmd : CGCmd {
  CGViewportCmd(CGViewport viewport, uint32_t viewportIndex)
    : CGCmd(Viewport), viewport(viewport), viewportIndex(viewportIndex) {}

  CGViewport viewport;
  uint32_t viewportIndex;
};

/// Set scissor command.
///
struct CGScissorCmd : CGCmd {
  explicit CGScissorCmd(CGScissor scissor, uint32_t viewportIndex)
    : CGCmd(Scissor), scissor(scissor), viewportIndex(viewportIndex) {}

  CGScissor scissor;
  uint32_t viewportIndex;
};

/// Set render target command.
///
struct CGTargetCmd : CGCmd {
  explicit CGTargetCmd(CGTarget* target) : CGCmd(Target), target(target) {}
  CGTarget* target;
};

/// Set descriptor table command.
///
struct CGDcTableCmd : CGCmd {
  CGDcTableCmd(uint32_t tableIndex, uint32_t allocIndex)
    : CGCmd(DcTable), tableIndex(tableIndex), allocIndex(allocIndex) {}

  uint32_t tableIndex;
  uint32_t allocIndex;
};

/// Set vertex buffer command.
///
struct CGVxBufferCmd : CGCmd {
  CGVxBufferCmd(CGBuffer* buffer, uint64_t offset, uint32_t inputIndex)
    : CGCmd(VxBuffer), buffer(buffer), offset(offset), inputIndex(inputIndex) {}

  CGBuffer* buffer;
  uint64_t offset;
  uint32_t inputIndex;
};

/// Set index buffer command.
///
struct CGIxBufferCmd : CGCmd {
  CGIxBufferCmd(CGBuffer* buffer, uint64_t offset, CGIndexType type)
    : CGCmd(IxBuffer), buffer(buffer), offset(offset), type(type) {}

  CGBuffer* buffer;
  uint64_t offset;
  CGIndexType type;
};

/// Draw command.
///
struct CGDrawCmd : CGCmd {
  CGDrawCmd(uint32_t vertexStart,
            uint32_t vertexCount,
            uint32_t baseInstance,
            uint32_t instanceCount)
            : CGCmd(Draw),
              vertexStart(vertexStart),
              vertexCount(vertexCount),
              baseInstance(baseInstance),
              instanceCount(instanceCount) {}

  uint32_t vertexStart;
  uint32_t vertexCount;
  uint32_t baseInstance;
  uint32_t instanceCount;
};

/// Draw indexed command.
///
struct CGDrawIxCmd : CGCmd {
  CGDrawIxCmd(uint32_t indexStart,
              uint32_t vertexCount,
              int32_t vertexOffset,
              uint32_t baseInstance,
              uint32_t instanceCount)
              : CGCmd(DrawIx),
                indexStart(indexStart),
                vertexCount(vertexCount),
                vertexOffset(vertexOffset),
                baseInstance(baseInstance),
                instanceCount(instanceCount) {}

  uint32_t indexStart;
  uint32_t vertexCount;
  int32_t vertexOffset;
  uint32_t baseInstance;
  uint32_t instanceCount;
};

/// Dispatch command.
///
struct CGDispatchCmd : CGCmd {
  explicit CGDispatchCmd(CGSize3 size) : CGCmd(Dispatch), size(size) {}
  CGSize3 size;
};

/// Clear color command.
///
struct CGClearClCmd : CGCmd {
  CGClearClCmd(CGColor value, uint32_t colorIndex)
    : CGCmd(ClearCl), value(value), colorIndex(colorIndex) {}

  CGColor value;
  uint32_t colorIndex;
};

/// Clear depth command.
///
struct CGClearDpCmd : CGCmd {
  explicit CGClearDpCmd(float value) : CGCmd(ClearDp), value(value) {}
  float value;
};

/// Clear stencil command.
///
struct CGClearScCmd : CGCmd {
  explicit CGClearScCmd(uint32_t value) : CGCmd(ClearSc), value(value) {}
  uint32_t value;
};

YF_NS_END

#endif // YF_CG_CMD_H
