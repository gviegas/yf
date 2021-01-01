//
// SG
// Camera.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "Camera.h"

using namespace SG_NS;
using namespace std;

class Camera::Impl {
 public:
  Impl(const Vec3f& origin, const Vec3f& target, float aspect) {
    // TODO
  }

  // TODO
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
