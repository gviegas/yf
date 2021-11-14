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

/// PBR Metallic-Roughness.
///
#ifdef MATERIAL_PBRMR
void getPbrmr(inout vec4 color, out vec3 f0, out vec3 f90, out float ar) {
  float metallic = material_.pbrFac[0];
  float roughness = material_.pbrFac[1];

#ifdef HAS_PBR_MAP
  // TODO: Select correct coordinate set
  vec4 metalRough = texture(pbrMap_, vertexIn_.texCoorSet0);
  metallic *= metalRough.b;
  roughness *= metalRough.g;
#endif

  vec3 ior = vec3(0.04);
  color.rgb = mix(color.rgb * (vec3(1.0) - ior), vec3(0.0), metallic);
  f0 = mix(ior, color.rgb, metallic);
  f90 = vec3(1.0);
  ar = roughness * roughness;
}
#endif

/// PBR Specular-Glossiness.
///
#ifdef MATERIAL_PBRSG
void getPbrsg(inout vec4 color, out vec3 f0, out vec3 f90, out float ar) {
  vec3 specular = vec3(material_.pbrFac);
  float glossiness = material_.pbrFac[3];

#ifdef HAS_PBR_MAP
  // TODO: Select correct coordinate set
  vec4 specGloss = texture(pbrMap_, vertexIn_.texCoordSet0);
  specular *= specGloss.rgb;
  glossiness *= specGloss.a;
#endif

  color.rgb = color.rgb * (1.0 - max(specular.r, max(specular.g, specular.b)));
  f0 = specular;
  f90 = vec3(1.0);
  ar = 1.0 - glossiness;
  ar *= ar;
}
#endif
