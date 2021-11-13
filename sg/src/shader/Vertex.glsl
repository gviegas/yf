//
// SG
// Vertex.glsl
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef VERT_STAGE
# error Invalid include
#endif

layout(location=0) in vec3 position_;
#ifdef HAS_NORMAL
layout(location=1) in vec3 normal_;
#endif
#ifdef HAS_TANGENT
layout(location=2) in vec4 tangent_;
#endif
#ifdef HAS_TEXCOORD0
layout(location=3) in vec2 texCoord0_;
#endif
#ifdef HAS_TEXCOORD1
layout(location=4) in vec2 texCoord1_;
#endif
#ifdef HAS_COLOR0
layout(location=5) in vec4 color0_;
#endif
#ifdef HAS_SKIN
layout(location=6) in uvec4 joints0_;
layout(location=7) in vec4 weights0_;
#endif

layout(location=0) out VertexIO {
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
} vertexOut_;

/// Gets vertex position.
///
vec4 getPosition() {
  const int i = gl_InstanceIndex;
  vec4 pos = vec4(position_, 1.0);

#ifdef HAS_SKIN
  mat4 skin = instance_.i[i].joints[joints0_.x] * weigths0_.x +
              instance_.i[i].joints[joints0_.y] * weights0_.y +
              instance_.i[i].joints[joints0_.z] * weights0_.z +
              instance_.i[i].joints[joints0_.w] * weights0_.w;
  pos = skin * pos;
#endif

  return instance_.i[i].m * pos;
}

/// Gets vertex normal.
///
#ifdef HAS_NORMAL
vec3 getNormal() {
  const int i = gl_InstanceIndex;
  vec3 norm = normal_;

#ifdef HAS_SKIN
  mat4 nskin = instance_.i[i].normJoints[joints0.x] * weights0_.x +
               instance_.i[i].normJoints[joints0.y] * weights0_.y +
               instance_.i[i].normJoints[joints0.z] * weights0_.z +
               instance_.i[i].normJoints[joints0.w] * weights0_.w;
  norm = normalize(mat3(nskin) * norm);
#endif

  return normalize(vec3(instance_.i[i].norm * vec4(norm, 0.0)));
}
#endif

/// Sets vertex output's position.
///
void setPosition(vec4 position) {
  vertexOut_.position = position.xyz / position.w;
}

/// Sets vertex output's normal.
///
void setNormal() {
#ifdef HAS_NORMAL
  vertexOut_.normal = getNormal();
#endif
}

/// Sets vertex output's tangent.
///
void setTangent() {
#ifdef HAS_TANGENT
  // TODO
#endif
}

/// Sets vertex output's tex. coord. #0.
///
void setTexCoord0() {
#ifdef HAS_TEXCOORD0
  vertexOut_.texCoord0 = texCoord0_;
#endif
}

/// Sets vertex output's tex. coord. #1.
///
void setTexCoord1() {
#ifdef HAS_TEXCOORD1
  vertexOut_.texCoord1 = texCoord1_;
#endif
}

/// Sets vertex output's color #0.
///
void setColor0() {
#ifdef HAS_COLOR0
  vertexOut_.color0 = color0_;
#endif
}

/// Sets vertex output's eye.
///
void setEye(vec4 position) {
  vertexOut_.eye = global_.v[3].xyz - position.xyz;
}
