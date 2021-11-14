//
// SG
// Fragment.glsl
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef FRAG_STAGE
# error Invalid include
#endif

layout(location=0) in VertexIO {
  vec3 position;
#ifdef HAS_NORMAL
  vec3 normal;
#endif
#ifdef HAS_TANGENT
  vec4 tangent;
#endif
#ifdef HAS_TEXCOORD0
  vec2 texCoord0;
#endif
#ifdef HAS_TEXCOORD1
  vec2 texCoord1;
#endif
#ifdef HAS_COLOR0
  vec4 color0;
#endif
  vec3 eye;
} vertexIn_;

layout(location=0) out vec4 color0_;
#ifdef HAS_COLOR_OUTPUT_1
layout(location=1) out vec4 color1_;
#endif
#ifdef HAS_COLOR_OUTPUT_2
layout(location=2) out vec4 color2_;
#endif
#ifdef HAS_COLOR_OUTPUT_3
layout(location=3) out vec4 color3_;
#endif
