//
// SG
// Skin.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_SKIN_H
#define YF_SG_SKIN_H

#include <cstdint>
#include <vector>
#include <memory>

#include "yf/sg/Defs.h"
#include "yf/sg/Matrix.h"

SG_NS_BEGIN

class Skin;

/// Joint.
///
class Joint : public Node {
 public:
  Joint(const Skin& skin, uint32_t index);
  ~Joint();

  const Skin& skin_;
  const uint32_t index_;
};

/// Skin.
///
class Skin {
 public:
  Skin(const std::vector<Mat4f>& bindPose,
       const std::vector<Mat4f>& inverseBind);

  ~Skin();

  /// The joints.
  ///
  const std::vector<Joint> joints() const;

  /// The inverse-bind matrix of each joint.
  ///
  const std::vector<Mat4f> inverseBind() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_SKIN_H
