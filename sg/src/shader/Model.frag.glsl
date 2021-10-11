//
// SG
// Model.frag.glsl
//
// Copyright © 2021 Gustavo C. Viegas.
//

#version 460 core

const float Pi = 3.14159265358979323846;
const float OneOverPi = 0.31830988618379067154;

const uint ColorTexBit      = 0x0100;
const uint MetalRoughTexBit = 0x0200;
const uint NormalTexBit     = 0x0400;
const uint OcclusionTexBit  = 0x0800;
const uint EmissiveTexBit   = 0x1000;

// TODO: Punctual lights
const vec3 LightDirection = vec3(-0.7527726527, -0.6199304199, -0.2214037214);
const vec3 LightIntensity = vec3(6.0);
const vec3 LightColor = vec3(1.0);

/// Check list data.
///
layout(set=1, binding=1) uniform Check {
  uint mask;
} check;

/// Material data.
///
layout(set=1, binding=2) uniform Material {
  vec4 colorFac;
  float metallicFac;
  float roughnessFac;
  float normalFac;
  float occlusionFac;
  vec3 emissiveFac;
} material;

/// Material textures.
///
layout(set=1, binding=3) uniform sampler2D colorTex;
layout(set=1, binding=4) uniform sampler2D metalRoughTex;
layout(set=1, binding=5) uniform sampler2D normalTex;
layout(set=1, binding=6) uniform sampler2D occlusionTex;
layout(set=1, binding=7) uniform sampler2D emissiveTex;

layout(location=0) in Vertex {
  vec3 position;
  vec3 normal;
  vec4 tangent;
  vec2 texCoord0;
  vec2 texCoord1;
  vec4 color0;
  vec3 eye;
  // TODO...
} vertex;

layout(location=0) out vec4 color0;

float microfacetD(float NdotH, float alphaRoughSq) {
  float fac = (NdotH * NdotH) * (alphaRoughSq - 1.0) + 1.0;
  return alphaRoughSq / (Pi * fac * fac);
}

float visibilityV(float NdotV, float NdotL, float alphaRoughSq) {
  float aDiff = 1.0 - alphaRoughSq;
  float GGXV = NdotL * sqrt(NdotV * NdotV * aDiff + alphaRoughSq);
  float GGXL = NdotV * sqrt(NdotL * NdotL * aDiff + alphaRoughSq);
  float GGX = GGXV + GGXL;
  return (GGX > 0.0) ? (0.5 / GGX) : (0.0);
}

vec3 fresnelF(vec3 f0, float VdotH) {
  const vec3 f90 = vec3(1.0);
  return f0 + (f90 - f0) * pow(1.0 - abs(VdotH), 5.0);
}

vec3 diffuseBRDF(vec3 color, vec3 fTerm) {
  return (1.0 - fTerm) * (color * OneOverPi);
}

vec3 specularBRDF(vec3 fTerm, float NdotV, float NdotL, float NdotH,
                  float alphaRoughSq) {
  return fTerm *
         visibilityV(NdotV, NdotL, alphaRoughSq) *
         microfacetD(NdotH, alphaRoughSq);
}

vec4 getColor() {
  vec4 clr = vec4(1.0);

  if ((check.mask & ColorTexBit) != 0)
    clr = textureLod(colorTex, vertex.texCoord0, 0.0);
  clr *= material.colorFac;

  float metallic = material.metallicFac;
  float roughness = material.roughnessFac;

  if ((check.mask & MetalRoughTexBit) != 0) {
    vec4 mr = textureLod(metalRoughTex, vertex.texCoord0, 0.0);
    metallic *= mr.b;
    roughness *= mr.g;
  }

  // TODO: normal/occlusion/emissive maps

  vec3 v = normalize(vertex.eye);
  vec3 l = normalize(-LightDirection);
  vec3 n = normalize(vertex.normal);
  vec3 h = normalize(l + v);

  float VdotH = max(dot(v, h), 0.0);
  float NdotV = max(dot(n, v), 0.0);
  float NdotL = max(dot(n, l), 0.0);
  float NdotH = max(dot(n, h), 0.0);

  vec3 ior = vec3(0.04);
  vec3 albedo = mix(clr.rgb * (vec3(1.0) - ior), vec3(0.0), metallic);
  vec3 f0 = mix(ior, clr.rgb, metallic);
  float alphaRoughSq = roughness * roughness;
  alphaRoughSq *= alphaRoughSq;

  vec3 fTerm = fresnelF(f0, VdotH);
  vec3 diffuse = diffuseBRDF(albedo, fTerm);
  vec3 specular = specularBRDF(fTerm, NdotV, NdotL, NdotH, alphaRoughSq);

  clr.xyz = (diffuse + specular) * LightIntensity * LightColor * NdotL;
  return clr;
}

void main() {
  color0 = vertex.color0 * getColor();
}
