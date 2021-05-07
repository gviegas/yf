//
// SG
// Skin.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_SKIN_H
#define YF_SG_SKIN_H

#include <cstddef>
#include <vector>
#include <memory>

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
  Skin(size_t jointN, const std::vector<Mat4f>& inverseBind);
  Skin();
  Skin(const Skin& other);
  Skin& operator=(const Skin& other);
  ~Skin();

  explicit operator bool() const;
  bool operator!() const;
  bool operator==(const Skin& other) const;
  bool operator!=(const Skin& other) const;

  size_t hash() const;

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
  std::shared_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_SKIN_H
