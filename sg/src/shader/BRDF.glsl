//
// SG
// BRDF.glsl
//
// Copyright © 2021 Gustavo C. Viegas.
//

#define PI          3.14159265358979323846
#define ONE_OVER_PI 0.31830988618379067154

/// Fresnel term (F).
///
vec3 fresnelF(vec3 f0, vec3 f90, float vdoth) {
  return f0 + (f90 - f0) * pow(1.0 - abs(vdoth), 5.0);
}

/// Visibility (V).
///
float visibilityV(float ndotv, float ndotl, float arar) {
  float diff = 1.0 - arar;
  float ggx = sqrt(ndotv * ndotv * diff + arar) * ndotl +
              sqrt(ndotl * ndotl * diff + arar) * ndotv;

  return (ggx > 0.0) ? (0.5 / ggx) : (0.0);
}

/// Microfacet (D).
///
float microfacetD(float ndoth, float arar) {
  float fac = (ndoth * ndoth) * (arar - 1.0) + 1.0;
  return arar / (PI * fac * fac);
}

/// Diffuse BRDF.
///
vec3 diffuseBRDF(vec3 color, vec3 F) {
  return (1.0 - F) * color * ONE_OVER_PI;
}

/// Specular BRDF.
///
vec3 specularBRDF(vec3 F, float ndotv, float ndotl, float ndoth, float arar) {
  return F * visibilityV(ndotv, ndotl, arar) * microfacetD(ndoth, arar);
}
