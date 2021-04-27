//
// SG
// Model16.vert.glsl
//
// Copyright © 2021 Gustavo C. Viegas.
//

#version 460 core

/// Global uniform.
///
layout(set=0, binding=0) uniform Glb {
  mat4 v;
  mat4 p;
  // TODO...
} glb;

/// Instance-specific uniform.
///
layout(set=1, binding=0) uniform Inst {
  mat4 m;
  mat4 mv;
  // TODO...
} inst[16];

layout(location=0) in vec3 pos;
layout(location=1) in vec3 norm;
layout(location=3) in vec2 tc0;
// TODO...

layout(location=0) out Vert {
  vec3 norm;
  vec2 tc0;
  // TODO...
} vert;

void main() {
  gl_Position = glb.p * inst[gl_InstanceIndex].mv * vec4(pos, 1.0);
  vert.norm = norm;
  vert.tc0 = tc0;
}
