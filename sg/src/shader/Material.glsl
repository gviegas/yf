//
// SG
// Material.glsl
//
// Copyright © 2021 Gustavo C. Viegas.
//

#define MATERIAL_BINDING 1

layout(std140, column_major) uniform;

/// Material uniform data.
///
layout(set=1, binding=MATERIAL_BINDING) uniform Material {
  vec4 colorFac;
  float alphaCutoff;
  int doubleSided;
#ifndef MATERIAL_UNLIT
  float normalFac;
  float occlusionFac;
  vec4 pbrFac;
  vec3 emissiveFac;

  float pad1;
#else

  float pad1, pad2;
#endif
} material_;

/// Texture maps.
///
#ifdef HAS_COLOR_MAP
# define COLOR_MAP_BINDING MATERIAL_BINDING + 1
layout(set=1, binding=COLOR_MAP_BINDING) uniform sampler2D colorMap_;
#else
# define COLOR_MAP_BINDING MATERIAL_BINDING
#endif

#ifdef HAS_PBR_MAP
# define PBR_MAP_BINDING COLOR_MAP_BINDING + 1
layout(set=1, binding=PBR_MAP_BINDING) uniform sampler2D pbrMap_;
#else
# define PBR_MAP_BINDING COLOR_MAP_BINDING
#endif

#ifdef HAS_NORMAL_MAP
# define NORMAL_MAP_BINDING PBR_MAP_BINDING + 1
layout(set=1, binding=NORMAL_MAP_BINDING) uniform sampler2D normalMap_;
#else
# define NORMAL_MAP_BINDING PBR_MAP_BINDING
#endif

#ifdef HAS_OCCLUSION_MAP
# define OCCLUSION_MAP_BINDING NORMAL_MAP_BINDING + 1
layout(set=1, binding=OCCLUSION_MAP_BINDING) uniform sampler2D occlusionMap_;
#else
# define OCCLUSION_MAP_BINDING NORMAL_MAP_BINDING
#endif

#ifdef HAS_EMISSIVE_MAP
# define EMISSIVE_MAP_BINDING OCCLUSION_MAP_BINDING + 1
layout(set=1, binding=EMISSIVE_MAP_BINDING) uniform sampler2D emissiveMap_;
#else
# define EMISSIVE_MAP_BINDING OCCLUSION_MAP_BINDING
#endif
