//
// SG
// Model.frag.glsl
//
// Copyright © 2021 Gustavo C. Viegas.
//

#version 460 core

/// Color texture's image sampler.
///
layout(set=1, binding=1) uniform sampler2D colorTex;

layout(location=0) in Vert {
  vec3 norm;
  vec2 tc0;
  // TODO...
} vert;

layout(location=0) out vec4 frag0;

void main() {
  frag0 = textureLod(colorTex, vert.tc0, 0.0);
}
