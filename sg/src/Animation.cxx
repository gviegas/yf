//
// SG
// Animation.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <stdexcept>

#include "Animation.h"

using namespace SG_NS;
using namespace std;

class Animation::Impl {
 public:
  Impl(const vector<Timeline>& inputs, const vector<Translation>& outT,
       const vector<Rotation>& outR, const vector<Scale>& outS)
    : inputs_(inputs), outT_(outT), outR_(outR), outS_(outS) { }

  bool play(std::chrono::nanoseconds elapsedTime) {
    if (completed_)
      return false;

    update(elapsedTime);
    return !completed_;
  }

  void stop() {
    time_ = time_.zero();
  }

  wstring name_;
  vector<Action> actions_;
  const vector<Timeline> inputs_;
  const vector<Translation> outT_;
  const vector<Rotation> outR_;
  const vector<Scale> outS_;

 private:
  chrono::duration<float> time_;
  bool completed_ = false;

  /// Updates the animation.
  ///
  void update(chrono::nanoseconds elapsedTime) {
    time_ += elapsedTime;

    // Get a pair of timeline indices defining the keyframes to interpolate
    auto getKeyframes = [&](const Timeline& input) {
      const float tm = time_.count();

      if (input.front() > tm)
        return pair<size_t, size_t>();

      if (input.back() < tm)
        return input.size() > 1 ?
               pair<size_t, size_t>(input.size()-2, input.size()-1) :
               pair<size_t, size_t>(input.size()-1, input.size()-1);

      size_t beg = 0;
      size_t end = input.size() - 1; // XXX: non-empty input
      size_t cur = (beg + end) >> 1;

      while (beg < end) {
        if (input[cur] < tm)
          beg = cur + 1;
        else if (input[cur] > tm)
          end = cur - 1;
        else
          break;

        cur = (beg + end) >> 1;
      }

      return input[cur] > tm ?
             pair<size_t, size_t>(cur-1, cur) :
             pair<size_t, size_t>(cur, cur+1);
    };

    // Update translation action
    auto updateT = [&](const Action& action) {
      // TODO
    };

    // Update rotation action
    auto updateR = [&](const Action& action) {
      // TODO
    };

    // Update scale action
    auto updateS = [&](const Action& action) {
      // TODO
    };

    for (const auto& act : actions_) {
      switch (act.type) {
      case T:
        updateT(act);
        break;
      case R:
        updateR(act);
        break;
      case S:
        updateS(act);
        break;
      }
    }

    // TODO
  }
};

Animation::Animation(const vector<Timeline>& inputs,
                     const vector<Translation>& outT,
                     const vector<Rotation>& outR,
                     const vector<Scale>& outS)
  : impl_(make_shared<Impl>(inputs, outT, outR, outS)) { }

Animation::Animation() : impl_(nullptr) { }

Animation::Animation(const Animation& other) : impl_(other.impl_) { }

Animation& Animation::operator=(const Animation& other) {
  impl_ = other.impl_;
  return *this;
}

Animation::~Animation() { }

Animation::operator bool() const {
  return impl_ != nullptr;
}

bool Animation::operator!() const {
  return impl_ == nullptr;
}

bool Animation::operator==(const Animation& other) const {
  return impl_ == other.impl_;
}

bool Animation::operator!=(const Animation& other) const {
  return impl_ != other.impl_;
}

wstring& Animation::name() {
  if (!impl_)
    throw runtime_error("Call to name() of invalid Animation");
  return impl_->name_;
}

const wstring& Animation::name() const {
  if (!impl_)
    throw runtime_error("Call to name() of invalid Animation");
  return impl_->name_;
}

vector<Animation::Action>& Animation::actions() {
  if (!impl_)
    throw runtime_error("Call to actions() of invalid Animation");
  return impl_->actions_;
}

const vector<Animation::Action>& Animation::actions() const {
  if (!impl_)
    throw runtime_error("Call to actions() of invalid Animation");
  return impl_->actions_;
}

const vector<Animation::Timeline>& Animation::inputs() const {
  if (!impl_)
    throw runtime_error("Call to inputs() of invalid Animation");
  return impl_->inputs_;
}

const vector<Animation::Translation>& Animation::outT() const {
  if (!impl_)
    throw runtime_error("Call to outT() of invalid Animation");
  return impl_->outT_;
}

const vector<Animation::Rotation>& Animation::outR() const {
  if (!impl_)
    throw runtime_error("Call to outR() of invalid Animation");
  return impl_->outR_;
}

const vector<Animation::Scale>& Animation::outS() const {
  if (!impl_)
    throw runtime_error("Call to outS() of invalid Animation");
  return impl_->outS_;
}

bool Animation::play(chrono::nanoseconds elapsedTime) {
  if (!impl_)
    throw runtime_error("Call to play() of invalid Animation");

  return impl_->play(elapsedTime);
}

void Animation::stop() {
  if (!impl_)
    throw runtime_error("Call to stop() of invalid Animation");

  impl_->stop();
}
