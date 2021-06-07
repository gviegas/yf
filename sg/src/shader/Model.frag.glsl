//
// SG
// Model.frag.glsl
//
// Copyright © 2021 Gustavo C. Viegas.
//

#version 460 core

const uint ColorTexBit      = 0x0100;
const uint MetalRoughTexBit = 0x0200;
const uint NormalTexBit     = 0x0400;
const uint OcclusionTexBit  = 0x0800;
const uint EmissiveTexBit   = 0x1000;

/// Check list data.
///
layout(set=1, binding=1) uniform Check {
  uint mask;
} check;

/// Material data.
///
layout(set=1, binding=3) uniform Material {
  vec4 colorFac;
  float metallicFac;
  float roughnessFac;
  float normalFac;
  float occlusionFac;
  vec3 emissiveFac;
} material;

/// Material textures.
///
layout(set=1, binding=4) uniform sampler2D colorTex;
layout(set=1, binding=5) uniform sampler2D metalRoughTex;
layout(set=1, binding=6) uniform sampler2D normalTex;
layout(set=1, binding=7) uniform sampler2D occlusionTex;
layout(set=1, binding=8) uniform sampler2D emissiveTex;

layout(location=0) in Vertex {
  vec3 position;
  vec4 tangent;
  vec3 normal;
  vec2 texCoord0;
  vec2 texCoord1;
  vec4 color0;
  vec3 eye;
  // TODO...
} vertex;

layout(location=0) out vec4 color0;

vec4 getColor() {
  vec4 clr = vec4(1.0);

  if ((check.mask & ColorTexBit) != 0)
    clr = textureLod(colorTex, vertex.texCoord0, 0.0);
  clr *= material.colorFac;

  // TODO...

  return clr;
}

void main() {
  color0 = vertex.color0 * getColor();
}
