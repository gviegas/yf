//
// SG
// Animation.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_ANIMATION_H
#define YF_SG_ANIMATION_H

#include <cstddef>
#include <memory>
#include <vector>

#include "yf/sg/Defs.h"
#include "yf/sg/Vector.h"
#include "yf/sg/Quaternion.h"

SG_NS_BEGIN

class Node;

/// Keyframe animation.
///
class Animation {
 public:
  /// Input (keyframe times).
  ///
  using Timeline = std::vector<float>;

  /// Output (node properties).
  ///
  using Translation = std::vector<Vec3f>;
  using Rotation = std::vector<Qnionf>;
  using Scale = std::vector<Vec3f>;

  /// Type of animated property.
  ///
  enum Type {
    T,
    R,
    S
  };

  /// Interpolation method.
  ///
  enum Method {
    Step,
    Linear,
    Cubic
  };

  /// Keyframe animation of a target node.
  ///
  struct Action {
    Node* target;
    Type type;
    Method method;
    size_t input;
    size_t output;
  };

  Animation(const std::vector<Timeline>& inputs,
            const std::vector<Translation>& outT,
            const std::vector<Rotation>& outR,
            const std::vector<Scale>& outS);
  Animation();
  Animation(const Animation& other);
  Animation& operator=(const Animation& other);
  ~Animation();

  explicit operator bool() const;
  bool operator!() const;
  bool operator==(const Animation& other) const;
  bool operator!=(const Animation& other) const;

  /// Actions.
  ///
  std::vector<Action>& actions();
  const std::vector<Action>& actions() const;

  /// Inputs.
  ///
  const std::vector<Timeline>& inputs() const;

  /// Outputs.
  ///
  const std::vector<Translation>& outT() const;
  const std::vector<Rotation>& outR() const;
  const std::vector<Scale>& outS() const;

 private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_ANIMATION_H
