//
// SG
// Skin.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <stdexcept>

#include "Skin.h"

using namespace SG_NS;
using namespace std;

class Skin::Impl {
 public:
  Impl(size_t jointN, const vector<Mat4f>& inverseBind)
    : joints_(jointN, nullptr), inverseBind_(inverseBind) {

    if (!inverseBind.empty() && inverseBind.size() != jointN)
      throw invalid_argument("Skin joint/inverse matrix count mismatch");
  }

  ~Impl() { }

  vector<Joint*> joints_{};
  vector<Mat4f> inverseBind_{};
};

Skin::Skin(size_t jointN, const vector<Mat4f>& inverseBind)
  : impl_(make_shared<Impl>(jointN, inverseBind)) { }

Skin::Skin() : impl_(nullptr) { }

Skin::Skin(const Skin& other) : impl_(other.impl_) { }

Skin& Skin::operator=(const Skin& other) {
  impl_ = other.impl_;
  return *this;
}

Skin::~Skin() { }

Skin::operator bool() const {
  return impl_ != nullptr;
}

bool Skin::operator!() const {
  return impl_ == nullptr;
}

bool Skin::operator==(const Skin& other) const {
  return impl_ == other.impl_;
}

bool Skin::operator!=(const Skin& other) const {
  return impl_ != other.impl_;
}

size_t Skin::hash() const {
  return std::hash<decltype(impl_)>()(impl_);
}

void Skin::setJoint(Joint& joint, size_t index) {
  if (!impl_)
    throw runtime_error("Call to setJoint() of invalid Skin");
  if (index >= impl_->joints_.size())
    throw runtime_error("Skin setJoint() index out of bounds");
  impl_->joints_[index] = &joint;
}

const vector<Joint*>& Skin::joints() const {
  if (!impl_)
    throw runtime_error("Call to joints() of invalid Skin");
  return impl_->joints_;
}

const vector<Mat4f>& Skin::inverseBind() const {
  if (!impl_)
    throw runtime_error("Call to inverseBind() of invalid Skin");
  return impl_->inverseBind_;
}
