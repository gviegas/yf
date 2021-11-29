//
// SG
// Animation.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <cfloat>
#include <cmath>
#include <utility>

#include "Animation.h"
#include "Node.h"

using namespace SG_NS;
using namespace std;

class Animation::Impl {
 public:
  Impl(const vector<Timeline>& inputs, const vector<Translation>& outT,
       const vector<Rotation>& outR, const vector<Scale>& outS)
    : inputs_(inputs), outT_(outT), outR_(outR), outS_(outS) { }

  bool play(chrono::nanoseconds elapsedTime) {
    if (completed_)
      return false;

    update(elapsedTime);
    return !completed_;
  }

  void stop() {
    time_ = time_.zero();
    completed_ = false;
  }

  wstring name_;
  vector<Action> actions_;
  const vector<Timeline> inputs_;
  const vector<Translation> outT_;
  const vector<Rotation> outR_;
  const vector<Scale> outS_;

 private:
  chrono::duration<float> time_{};
  bool completed_ = false;

  /// Get a pair of timeline indices defining the keyframes to interpolate.
  ///
  pair<size_t, size_t> getKeyframes(const Timeline& input, float tm) {
    if (input.front() > tm)
      return {0, 0};

    if (input.back() < tm)
      return {input.size() - 1, input.size() - 1};

    size_t beg = 0;
    size_t end = input.size();
    size_t mid;

    while (beg < end) {
      mid = (beg + end) >> 1;
      if (input[mid] < tm)
        beg = mid + 1;
      else if (input[mid] > tm)
        end = mid;
      else
        break;
    }

    if (input[mid] > tm)
      return {mid - 1, mid};
    return {mid, mid + 1};
  }

  /// Linear interpolation for 'Vec3f'.
  ///
  Vec3f lerp(const Vec3f& v1, const Vec3f& v2, float f) {
    return Vec3f{(1.0f - f) * v1[0] + f * v2[0],
                 (1.0f - f) * v1[1] + f * v2[1],
                 (1.0f - f) * v1[2] + f * v2[2]};
  }

  /// Spherical linear interpolation for 'Qnionf'.
  ///
  Qnionf slerp(const Qnionf& q1, const Qnionf& q2, float f) {
    float d = dot(q1.v(), q2.v());
    float k = 1.0f;

    if (d > (1.0f - FLT_EPSILON)) {
      const float r = (1.0f - f) * q1.r() + f * q2.r();
      const Vec3f v = lerp(q1.v(), q2.v(), f);
      return Qnionf(r, v);
    }

    if (d < 0.0f) {
      d = -d;
      k = -k;
    }

    const float a = acos(d);
    const float s = sin(a);
    const float s1 = sin((1.0f - f) * a);
    const float s2 = sin(f * a);

    const float r = (q1.r() * s1 + q2.r() * s2 * k) / s;
    const Vec3f v = (q1.v() * s1 + q2.v() * s2 * k) / s;
    return Qnionf(r, v);
  }

  /// Updates a translation action.
  ///
  void updateT(const Action& action, float tm) {
    const auto& inp = inputs_[action.input];
    const auto& out = outT_[action.output];
    const auto seq = getKeyframes(inp, tm);
    auto node = action.target;

    switch (action.method) {
    case Step:
      if (tm - inp[seq.first] < inp[seq.second] - tm)
        node->setT(out[seq.first]);
      else
        node->setT(out[seq.second]);
      break;
    case Linear:
      if (seq.first != seq.second) {
        auto f = (tm - inp[seq.first]) / (inp[seq.second] - inp[seq.first]);
        node->setT(lerp(out[seq.first], out[seq.second], f));
      } else {
        node->setT(out[seq.first]);
      }
      break;
    case Cubic:
      // TODO
      break;
    }
  }

  /// Updates a rotation action.
  ///
  void updateR(const Action& action, float tm) {
    const auto& inp = inputs_[action.input];
    const auto& out = outR_[action.output];
    const auto seq = getKeyframes(inp, tm);
    auto node = action.target;

    switch (action.method) {
    case Step:
      if (tm - inp[seq.first] < inp[seq.second] - tm)
        node->setR(out[seq.first]);
      else
        node->setR(out[seq.second]);
      break;
    case Linear:
      if (seq.first != seq.second) {
        auto f = (tm - inp[seq.first]) / (inp[seq.second] - inp[seq.first]);
        node->setR(slerp(out[seq.first], out[seq.second], f));
      } else {
        node->setR(out[seq.first]);
      }
      break;
    case Cubic:
      // TODO
      break;
    }
  }

  /// Updates a scale action.
  ///
  void updateS(const Action& action, float tm) {
    const auto& inp = inputs_[action.input];
    const auto& out = outS_[action.output];
    const auto seq = getKeyframes(inp, tm);
    auto node = action.target;

    switch (action.method) {
    case Step:
      if (tm - inp[seq.first] < inp[seq.second] - tm)
        node->setS(out[seq.first]);
      else
        node->setS(out[seq.second]);
      break;
    case Linear:
      if (seq.first != seq.second) {
        auto f = (tm - inp[seq.first]) / (inp[seq.second] - inp[seq.first]);
        node->setS(lerp(out[seq.first], out[seq.second], f));
      } else {
        node->setS(out[seq.first]);
      }
      break;
    case Cubic:
      // TODO
      break;
    }
  }

  /// Updates the animation.
  ///
  void update(chrono::nanoseconds elapsedTime) {
    time_ += elapsedTime;
    const float tm = time_.count();

    size_t completeN = 0;

    for (const auto& act : actions_) {
      if (inputs_[act.input].back() <= tm)
        completeN++;

      switch (act.type) {
      case T:
        updateT(act, tm);
        break;
      case R:
        updateR(act, tm);
        break;
      case S:
        updateS(act, tm);
        break;
      }
    }

    completed_ = completeN == actions_.size();
  }
};

Animation::Animation(const vector<Timeline>& inputs,
                     const vector<Translation>& outT,
                     const vector<Rotation>& outR,
                     const vector<Scale>& outS)
  : impl_(make_unique<Impl>(inputs, outT, outR, outS)) { }

Animation::~Animation() { }

wstring& Animation::name() {
  return impl_->name_;
}

const wstring& Animation::name() const {
  return impl_->name_;
}

vector<Animation::Action>& Animation::actions() {
  return impl_->actions_;
}

const vector<Animation::Action>& Animation::actions() const {
  return impl_->actions_;
}

const vector<Animation::Timeline>& Animation::inputs() const {
  return impl_->inputs_;
}

const vector<Animation::Translation>& Animation::outT() const {
  return impl_->outT_;
}

const vector<Animation::Rotation>& Animation::outR() const {
  return impl_->outR_;
}

const vector<Animation::Scale>& Animation::outS() const {
  return impl_->outS_;
}

bool Animation::play(chrono::nanoseconds elapsedTime) {
  return impl_->play(elapsedTime);
}

void Animation::stop() {
  impl_->stop();
}
