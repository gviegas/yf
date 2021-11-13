//
// SG
// Instance.glsl
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef INST_N
# error INST_N not defined
#endif

#if defined(HAS_SKIN) && !defined(JOINT_N)
# error JOINT_N not defined
#endif

layout(std140, column_major) uniform;

/// Single instance.
///
struct PerInstance {
  mat4 m;
  mat4 mv;
  mat4 norm;

#ifdef HAS_SKIN
  mat4 joints[JOINT_N];
  mat4 normJoints[JOINT_N];
#endif
};

/// Instance uniform data.
///
layout(set=1, binding=0) uniform Instance {
  PerInstance i[INST_N];
} instance_;
