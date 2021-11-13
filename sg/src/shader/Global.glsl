//
// SG
// Global.glsl
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef VPORT_N
# error VPORT_N not defined
#endif

layout(std140, column_major) uniform;

/// Viewport.
///
struct Viewport {
  float x;
  float y;
  float width;
  float height;
  float zNear;
  float zFar;

  float pad1, pad2;
};

/// Global uniform data.
///
layout(set=0, binding=0) uniform Global {
  mat4 v;
  mat4 p;
  mat4 vp;
  mat4 o;
  Viewport vport[VPORT_N];
} global_;
