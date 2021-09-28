//
// CG
// Frag.frag
//
// Copyright © 2021 Gustavo C. Viegas.
//

#version 460 core

layout(set=0, binding=1) uniform sampler2D isampler;

layout(location=0) in Vertex {
  vec2 texCoord;
} vertex;

layout(location=0) out vec4 color;

void main() {
  color = texture(isampler, vertex.texCoord);
}
