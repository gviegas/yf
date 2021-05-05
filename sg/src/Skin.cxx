//
// SG
// Skin.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <typeinfo>
#include <stdexcept>

#include "Skin.h"

using namespace SG_NS;
using namespace std;

class Skin::Impl {
 public:
  Impl(const vector<Mat4f>& bindPose, const vector<Mat4f>& inverseBind)
    : joints_(), inverseBind_(inverseBind) {

    if (bindPose.empty())
      throw invalid_argument("Cannot create skin with no joints");

    if (!inverseBind.empty() && inverseBind.size() != bindPose.size())
      throw invalid_argument("Skin pose/inverse matrix count mismatch");

    for (const auto& m : bindPose) {
      joints_.push_back({});
      joints_.back().transform() = m;
    }
  }

  /*
  Impl(Skin& skin, const Skin& other)
    : skin_(skin), joints_(), inverseBind_(other.inverseBind()) {

    for (const auto& ojt : other.joints()) {
      joints_.push_back({skin, ojt.index_});
      auto& jt = joints_.back();
      jt.transform() = ojt.transform();
      jt.name() = ojt.name();
    }

    // XXX: copying only the relationships between the joints themselves
    for (size_t i = 0; i < joints_.size(); ++i) {
      auto& jt = joints_[i];
      const auto& ojt = other.joints()[i];

      const auto opn = ojt.parent();
      if (opn && typeid(*opn) == typeid(Joint)) {
        const auto& opj = *static_cast<const Joint*>(opn);
        if (&opj.skin_ == &ojt.skin_)
          joints_[opj.index_].insert(jt);
      }
    }
  }
  */

  ~Impl() { }

  vector<Joint> joints_{};
  vector<Mat4f> inverseBind_{};
};

Skin::Skin(const vector<Mat4f>& bindPose, const vector<Mat4f>& inverseBind)
  : impl_(make_shared<Impl>(bindPose, inverseBind)) { }

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

const vector<Joint>& Skin::joints() const {
  if (!impl_)
    throw runtime_error("Call to joints() of invalid Skin");
  return impl_->joints_;
}

const vector<Mat4f>& Skin::inverseBind() const {
  if (!impl_)
    throw runtime_error("Call to inverseBind() of invalid Skin");
  return impl_->inverseBind_;
}
