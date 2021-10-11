//
// SG
// Model.vert
//
// Copyright © 2021 Gustavo C. Viegas.
//

#version 460 core

#ifndef INSTANCE_N
# error "INSTANCE_N not defined"
#endif

#ifndef JOINT_N
# error "JOINT_N not defined"
#endif

const uint NormalBit    = 0x01;
const uint TangentBit   = 0x02;
const uint TexCoord0Bit = 0x04;
const uint TexCoord1Bit = 0x08;
const uint Color0Bit    = 0x10;
const uint SkinBit      = 0x20;

/// Global data.
///
layout(set=0, binding=0) uniform Global {
  mat4 v;
  mat4 p;
  mat4 vp;
  // TODO...
} global;

/// Per-instance data.
///
layout(set=1, binding=0) uniform Instance {
  mat4 m;
  mat4 mv;
  mat4 mvp;
  mat4 nm;
  mat4 jm[JOINT_N];
  mat4 njm[JOINT_N];
  // TODO...
} instance[INSTANCE_N];

/// Check list data.
///
layout(set=1, binding=1) uniform Check {
  uint mask;
} check;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec4 tangent;
layout(location=3) in vec2 texCoord0;
layout(location=4) in vec2 texCoord1;
layout(location=5) in vec4 color0;
layout(location=6) in uvec4 joints0;
layout(location=7) in vec4 weights0;

layout(location=0) out Vertex {
  vec3 position;
  vec3 normal;
  vec4 tangent;
  vec2 texCoord0;
  vec2 texCoord1;
  vec4 color0;
  vec3 eye;
  // TODO...
} vertex;

vec4 getPosition() {
  vec4 pos = vec4(position, 1.0);

  if ((check.mask & SkinBit) != 0) {
    const int i = gl_InstanceIndex;
    mat4 sm = weights0.x * instance[i].jm[joints0.x] +
              weights0.y * instance[i].jm[joints0.y] +
              weights0.z * instance[i].jm[joints0.z] +
              weights0.w * instance[i].jm[joints0.w];
    pos = sm * pos;
  }

  return pos;
}

vec3 getNormal() {
  vec3 norm = normal;

  if ((check.mask & SkinBit) != 0) {
    const int i = gl_InstanceIndex;
    mat4 nsm = weights0.x * instance[i].njm[joints0.x] +
               weights0.y * instance[i].njm[joints0.y] +
               weights0.z * instance[i].njm[joints0.z] +
               weights0.w * instance[i].njm[joints0.w];
    norm = normalize(mat3(nsm) * norm);
  }

  return norm;
}

void setVertex(vec4 pos) {
  vertex.position = pos.xyz / pos.w;

  if ((check.mask & NormalBit) != 0)
    vertex.normal =
      normalize(vec3(instance[gl_InstanceIndex].nm * vec4(getNormal(), 0.0)));
  else
    vertex.normal = vec3(0.0);

  if ((check.mask & TangentBit) != 0)
    vertex.tangent = tangent;
  else
    vertex.tangent = vec4(0.0);

  if ((check.mask & TexCoord0Bit) != 0)
    vertex.texCoord0 = texCoord0;
  else
    vertex.texCoord0 = vec2(0.0);

  if ((check.mask & TexCoord1Bit) != 0)
    vertex.texCoord1 = texCoord1;
  else
    vertex.texCoord1 = vec2(0.0);

  if ((check.mask & Color0Bit) != 0)
    vertex.color0 = color0;
  else
    vertex.color0 = vec4(1.0);

  vertex.eye = global.v[3].xyz - pos.xyz;
}

void main() {
  vec4 pos = instance[gl_InstanceIndex].m * getPosition();
  setVertex(pos);
  gl_Position = global.vp * pos;
}
