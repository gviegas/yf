//
// SG
// Camera.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <cmath>
#include <cstdint>
#include <algorithm>

#include "yf/Except.h"

#include "Camera.h"

using namespace SG_NS;
using namespace std;

class Camera::Impl {
 public:
  Impl(const Vec3f& origin, const Vec3f& target, float aspect)
    : pos_(origin), dir_(target - origin), aspect_(aspect),
      zoom_(fovMax), pending_(None) {

    if (fabsf(dir_.length()) < 1e-6f)
      throw invalid_argument("Camera origin and target vectors must differ");
    if (aspect_ <= 0.0f)
      throw invalid_argument("Camera aspect must be greater than zero");

    dir_.normalize();
    turnX_ = ::acosf(dot(dir_, worldUp));
    updateView();
    updateProj();
    viewProj_ = proj_ * view_;
  }

 private:
  void updateView() {
    view_ = lookAt(pos_, pos_ + dir_, worldUp);
    pending_ &= ~View;
    pending_ |= ViewProj;
  }

  void updateProj() {
    zoom_ = clamp(zoom_, fovMin, fovMax);
    proj_ = perspective(zoom_, aspect_, 0.1f, 100.0f);
    pending_ &= ~Proj;
    pending_ |= ViewProj;
  }

  using Mask = uint32_t;
  enum Flags : uint32_t {
    None     = 0,
    View     = 0x01,
    Proj     = 0x02,
    ViewProj = 0x04
  };

  static constexpr float fovMin = 0.07957747154594767f;
  static constexpr float fovMax = M_PI_4;
  static constexpr float turnMin = 0.0001f;
  static constexpr float turnMax = M_PI;
  static constexpr Vec3f worldUp{0.0f, -1.0f, 0.0f};

  Vec3f pos_{};
  Vec3f dir_{};
  float aspect_ = 0.0f;
  float turnX_ = 0.0f;
  float zoom_ = 0.0f;
  Mat4f view_{};
  Mat4f proj_{};
  Mat4f viewProj_{};
  Mask pending_ = None;
};

Camera::Camera(const Vec3f& origin, const Vec3f& target, float aspect)
  : impl_(make_unique<Impl>(origin, target, aspect)) { }

void Camera::place(const Vec3f& position) {
  // TODO
}

void Camera::point(const Vec3f& position) {
  // TODO
}

void Camera::moveForward(float delta) {
  // TODO
}

void Camera::moveBackward(float delta) {
  // TODO
}

void Camera::moveUp(float delta) {
  // TODO
}

void Camera::moveDown(float delta) {
  // TODO
}

void Camera::moveLeft(float delta) {
  // TODO
}

void Camera::moveRight(float delta) {
  // TODO
}

void Camera::turnUp(float delta) {
  // TODO
}

void Camera::turnDown(float delta) {
  // TODO
}

void Camera::turnLeft(float delta) {
  // TODO
}

void Camera::turnRight(float delta) {
  // TODO
}

void Camera::zoomIn(float delta) {
  // TODO
}

void Camera::zoomOut(float delta) {
  // TODO
}

void Camera::adjust(float aspect) {
  // TODO
}

const Mat4f& Camera::transform() {
  // TODO
}

const Mat4f& Camera::view() {
  // TODO
}

const Mat4f& Camera::projection() {
  // TODO
}
