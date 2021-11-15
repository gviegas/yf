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
void getPbr(inout vec4 color, out vec3 f0, out vec3 f90, out float ar) {
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
void getPbr(inout vec4 color, out vec3 f0, out vec3 f90, out float ar) {
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

#ifdef MATERIAL_UNLIT
void getPbr(inout vec4 color, out vec3 f0, out vec3 f90, out float ar) { }
#endif

/// Lighting.
///
void applyLights(inout vec4 color, vec3 f0, vec3 f90, float ar,
                 vec3 n, vec3 v, float ndotv) {

  vec3 albedo = vec3(0.0);

  for (uint i = 0; i < LIGHT_N; i++) {
    if (light_.l[i].notUsed != 0)
      break;

    vec3 l;
    float dist = 0.0001;
    float atten = 1.0;

    switch (light_.l[i].lightType) {
    case LIGHT_POINT:
      l = vec3(light_.l[i].position - vertexIn_.position);
      dist = max(length(l), dist);
      l /= dist;
      atten = attenuation(dist, light_.l[i].range);
      break;

    case LIGHT_SPOT:
      l = vec3(light_.l[i].position - vertexIn_.position);
      dist = max(length(l), dist);
      l /= dist;
      atten = attenuation(dist, light_.l[i].range);
      atten *= attenuation(light_.l[i].direction, l, light_.l[i].angularScale,
                           light_.l[i].angularOffset);
      break;

    case LIGHT_DIRECT:
      l = -light_.l[i].direction;
      break;
    }

    float ndotl = max(dot(n, l), 0.0);

    if (ndotl == 0.0 && ndotv == 0.0)
      continue;

    vec3 h = normalize(l + v);
    float ndoth = max(dot(n, h), 0.0);
    float vdoth = max(dot(v, h), 0.0);

    vec3 F = fresnelF(f0, f90, vdoth);
    vec3 diffuse = diffuseBRDF(color.rgb, F);
    vec3 specular = specularBRDF(F, ndotv, ndotl, ndoth, ar * ar);

    vec3 light = light_.l[i].color * light_.l[i].intensity * atten * ndotl;

    albedo += (diffuse + specular) * light;
  }

  color.rgb = albedo;
}

/// Gets fragment color.
///
vec4 getColor() {
  vec4 color = material_.colorFac;

#ifdef HAS_COLOR_MAP
  // TODO: Select correct coordinate set
  color *= texture(colorMap_, vertexIn_.texCoord0);
#endif

#ifdef HAS_COLOR0
  color *= vertexIn_.color0;
#endif

#if defined(ALPHA_OPAQUE)
  color.a = 1.0;
#elif defined(ALPHA_MASK)
  // TODO
# error Unimplemented
#endif

#ifdef MATERIAL_UNLIT
  return color;
#endif

  vec3 f0, f90;
  float ar;
  getPbr(color, f0, f90, ar);

  vec3 n;
#ifdef HAS_NORMAL
  n = normalize(vertexIn_.normal);
#else
  // TODO
# error Unimplemented
#endif

  vec3 v = normalize(vertexIn_.eye);
  float ndotv = max(dot(n, v), 0.0);
  applyLights(color, f0, f90, ar, n, v, ndotv);

#ifdef HAS_NORMAL_MAP
  // TODO
# error Unimplemented
#endif

#ifdef HAS_OCCLUSION_MAP
  // TODO
# error Unimplemented
#endif

#ifdef HAS_EMISSIVE_MAP
  // TODO
# error Unimplemented
#endif

  return color;
}
