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

//
// Joint
//

Joint::Joint(const Skin& skin, size_t index) : skin_(skin), index_(index) { }

Joint::~Joint() { }

//
// Skin
//

class Skin::Impl {
 public:
  Impl(Skin& skin, const vector<Mat4f>& bindPose,
       const vector<Mat4f>& inverseBind)
    : skin_(skin), joints_(), inverseBind_(inverseBind) {

    if (bindPose.empty())
      throw invalid_argument("Cannot create skin with no joints");

    if (!inverseBind.empty() && inverseBind.size() != bindPose.size())
      throw invalid_argument("Skin pose/inverse matrix count mismatch");

    for (const auto& m : bindPose) {
      joints_.push_back({skin_, joints_.size()});
      joints_.back().transform() = m;
    }
  }

  ~Impl() { }

  Skin& skin_;
  vector<Joint> joints_{};
  vector<Mat4f> inverseBind_{};
};

Skin::Skin(const vector<Mat4f>& bindPose, const vector<Mat4f>& inverseBind)
  : impl_(make_unique<Impl>(*this, bindPose, inverseBind)) { }

Skin::~Skin() { }

const vector<Joint>& Skin::joints() const {
  return impl_->joints_;
}

const vector<Mat4f>& Skin::inverseBind() const {
  return impl_->inverseBind_;
}
