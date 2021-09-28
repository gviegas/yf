//
// CG
// Vert.vert
//
// Copyright © 2021 Gustavo C. Viegas.
//

#version 460 core

layout(set=0, binding=0) uniform UBuffer {
  mat4 m;
} ubuffer;

layout(location=0) in vec3 position;
layout(location=1) in vec2 texCoord;

layout(location=0) out Vertex {
  vec2 texCoord;
} vertex;

void main() {
  gl_Position = ubuffer.m * vec4(position, 1.0);
  vertex.texCoord = texCoord;
}
