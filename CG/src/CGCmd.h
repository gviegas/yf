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
    GraphState,
    CompState,
    Viewport,
    Scissor,
    Target,
    DcTable,
    VxBuffer,
    IxBuffer,
    Draw,
    DrawIx,
    Dispatch,
    ClearCol,
    ClearDep,
    ClearSten
  };

  explicit CGCmd(Cmd cmd) : cmd(cmd) {}
  virtual ~CGCmd() = 0;

  const Cmd cmd;
};

CGCmd::~CGCmd() {}

/// Set graphics state command.
///
struct CGGraphStateCmd : CGCmd {
  explicit CGGraphStateCmd(CGGraphState* state)
    : CGCmd(GraphState), state(state) {}

  CGGraphState* state;
};

/// Set compute state command.
///
struct CGCompStateCmd : CGCmd {
  explicit CGCompStateCmd(CGCompState* state)
    : CGCmd(CompState), state(state) {}

  CGCompState* state;
};

/// Set viewport command.
///
struct CGViewportCmd : CGCmd {
  explicit CGViewportCmd(CGViewport viewport)
    : CGCmd(Viewport), viewport(viewport) {}

  CGViewport viewport;
};

/// Set scissor command.
///
struct CGScissorCmd : CGCmd {
  explicit CGScissorCmd(CGScissor scissor) : CGCmd(Scissor), scissor(scissor) {}
  CGScissor scissor;
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
  CGVxBufferCmd(uint32_t inputIndex, CGBuffer* buffer, uint64_t offset)
    : CGCmd(VxBuffer), inputIndex(inputIndex), buffer(buffer), offset(offset) {}

  uint32_t inputIndex;
  CGBuffer* buffer;
  uint64_t offset;
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
struct CGClearColCmd : CGCmd {
  CGClearColCmd(uint32_t colorIndex, CGColor value)
    : CGCmd(ClearCol), colorIndex(colorIndex), value(value) {}

  uint32_t colorIndex;
  CGColor value;
};

/// Clear depth command.
///
struct CGClearDepCmd : CGCmd {
  explicit CGClearDepCmd(float value) : CGCmd(ClearDep), value(value) {}
  float value;
};

/// Clear stencil command.
///
struct CGClearStenCmd : CGCmd {
  explicit CGClearStenCmd(uint32_t value) : CGCmd(ClearSten), value(value) {}
  uint32_t value;
};

YF_NS_END

#endif // YF_CG_CMD_H
