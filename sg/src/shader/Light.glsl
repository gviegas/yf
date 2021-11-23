//
// SG
// Light.glsl
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef LIGHT_N
# error LIGHT_N not defined
#endif

#if LIGHT_N < 1
# error LIGHT_N must be greater than zero
#endif

#define LIGHT_POINT  0
#define LIGHT_SPOT   1
#define LIGHT_DIRECT 2

layout(std140, column_major) uniform;

/// Light source.
///
struct LightSource {
  int notUsed;
  int lightType;
  float intensity;
  float range;
  vec3 color;
  float angularScale;
  vec3 position;
  float angularOffset;
  vec3 direction;

  float pad1;
};

/// Light uniform data.
///
layout(set=0, binding=1) uniform Light {
  LightSource l[LIGHT_N];
} light_;

/// Range attenuation
///
float attenuation(float dist, float range) {
  if (range > 0.0)
    return clamp(1.0 - pow(dist / range, 4.0), 0.0, 1.0) / pow(dist, 2.0);
  return 1.0 / pow(dist, 2.0);
}

/// Angular attenuation.
///
float attenuation(vec3 dir, vec3 l, float scale, float offset) {
  float fac = clamp(dot(dir, -l) * scale + offset, 0.0, 1.0);
  return fac * fac;
}
