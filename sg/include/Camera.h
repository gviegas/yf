//
// SG
// Camera.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_CAMERA_H
#define YF_SG_CAMERA_H

#include <memory>

#include "yf/sg/Defs.h"
#include "yf/sg/Vector.h"
#include "yf/sg/Matrix.h"

SG_NS_BEGIN

/// Camera.
///
class Camera {
 public:
  Camera(const Vec3f& origin, const Vec3f& target, float aspect);
  Camera(const Camera& other);
  Camera& operator=(const Camera& other);
  ~Camera();

  /// Places the camera at a given position.
  ///
  void place(const Vec3f& position);

  /// Points the camera towards a given position.
  ///
  void point(const Vec3f& position);

  /// Moves the camera forward.
  ///
  void moveForward(float delta);

  /// Moves the camera backward.
  ///
  void moveBackward(float delta);

  /// Moves the camera upward.
  ///
  void moveUp(float delta);

  /// Moves the camera downward.
  ///
  void moveDown(float delta);

  /// Moves the camera leftward.
  ///
  void moveLeft(float delta);

  /// Moves the camera rightward.
  ///
  void moveRight(float delta);

  /// Turns the camera up.
  ///
  void turnUp(float delta);

  /// Turns the camera down.
  ///
  void turnDown(float delta);

  /// Turns the camera to the left.
  ///
  void turnLeft(float delta);

  /// Turns the camera to the right.
  ///
  void turnRight(float delta);

  /// Zooms the camera in.
  ///
  void zoomIn(float delta);

  /// Zooms the camera out.
  ///
  void zoomOut(float delta);

  /// Adjusts the camera to a given aspect ratio.
  ///
  void adjust(float aspect);

  /// Gets the view-projection matrix.
  ///
  const Mat4f& transform() const;

  /// Gets the view matrix.
  ///
  const Mat4f& view() const;

  /// Gets the projection matrix.
  ///
  const Mat4f& projection() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_CAMERA_H
