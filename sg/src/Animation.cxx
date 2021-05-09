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

  wstring name_;
  vector<Action> actions_;
  vector<Timeline> inputs_;
  vector<Translation> outT_;
  vector<Rotation> outR_;
  vector<Scale> outS_;
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
