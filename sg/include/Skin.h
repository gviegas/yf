//
// SG
// Skin.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_SKIN_H
#define YF_SG_SKIN_H

#include <cstddef>
#include <memory>
#include <vector>

#include "yf/sg/Defs.h"
#include "yf/sg/Node.h"

SG_NS_BEGIN

/// Joint.
///
class Joint : public Node {};

/// Skin.
///
class Skin {
 public:
  using Ptr = std::unique_ptr<Skin>;

  Skin(size_t jointN, const std::vector<Mat4f>& inverseBind);
  ~Skin();

  /// Sets a skin joint.
  ///
  void setJoint(Joint& joint, size_t index);

  /// The joints.
  ///
  const std::vector<Joint*>& joints() const;

  /// The inverse-bind matrix of each joint.
  ///
  const std::vector<Mat4f>& inverseBind() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_SKIN_H
