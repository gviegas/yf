//
// SG
// Main.frag
//
// Copyright © 2021 Gustavo C. Viegas.
//

#version 460 core

#extension GL_GOOGLE_include_directive : require

#define FRAGMENT_STAGE

#include "BRDF.glsl"
#include "Material.glsl"
#include "Light.glsl"
#include "Fragment.glsl"

void main() {
  writeColor0();
  writeColor1();
  writeColor2();
  writeColor3();
}
