//
// SG
// Main.vert
//
// Copyright © 2021 Gustavo C. Viegas.
//

#version 460 core

#extension GL_GOOGLE_include_directive : require

#define VERT_STAGE

#include "Global.glsl"
#include "Instance.glsl"
#include "Vertex.glsl"

void main() {
  vec4 pos = getPosition();
  gl_Position = global_.vp * pos;

  setPosition(pos);
  setNormal();
  setTangent();
  setTexCoord0();
  setTexCoord1();
  setColor0();
  setEye(pos);
}
