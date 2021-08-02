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
  : impl_(make_unique<Impl>(jointN, inverseBind)) { }

Skin::~Skin() { }

void Skin::setJoint(Joint& joint, size_t index) {
  if (index >= impl_->joints_.size())
    throw runtime_error("Skin setJoint() index out of bounds");

  impl_->joints_[index] = &joint;
}

const vector<Joint*>& Skin::joints() const {
  return impl_->joints_;
}

const vector<Mat4f>& Skin::inverseBind() const {
  return impl_->inverseBind_;
}
