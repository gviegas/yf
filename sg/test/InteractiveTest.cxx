//
// SG
// InteractiveTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "InteractiveTest.h"
#include "Camera.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

constexpr float deltaM = 16.0f;
constexpr float deltaT = 2.0f;
constexpr float deltaZ = 5.0f;

InteractiveTest::Input InteractiveTest::input{};

InteractiveTest::InteractiveTest(wstring&& name, uint32_t width,
                                 uint32_t height)
  : Test(move(name)),
    window_(WS_NS::createWindow(width, height, name, WS_NS::Window::Resizable |
                                                     WS_NS::Window::Hidden)),
    view_(window_.get()), scene_{}, object_{} { }

void InteractiveTest::setScene(Scene* scene) {
  scene_ = scene;
  if (scene_)
    view_.swapScene(*scene);
}

void InteractiveTest::setObject(Node* node) {
  object_ = node;
}

void InteractiveTest::update(Scene& scene, const View::UpdateFn& fn) {
  scene_ = &scene;
  window_->open();

  WS_NS::onPtMotion(onMotion);
  WS_NS::onPtButton(onButton);
  WS_NS::onKbKey(onKey);

  bool camMode = true;

  view_.loop(scene, 60, [&](auto elapsedTime) {
    if (input.quit || !scene_)
      return false;

    auto& cam = scene_->camera();

    if (input.mode) {
      camMode = !camMode;
      input.mode = false;
    }

    const auto dt = chrono::duration<float>(elapsedTime).count();

    if (input.primary) {
      if (input.x < input.prevX) {
        cam.turnLeft(0.25f * (input.prevX - input.x) * dt);
        input.prevX = input.x;
      } else if (input.x > input.prevX) {
        cam.turnRight(0.25f * (input.x - input.prevX) * dt);
        input.prevX = input.x;
      }
      if (input.y < input.prevY) {
        cam.turnUp(0.25f * (input.prevY - input.y) * dt);
        input.prevY = input.y;
      } else if (input.y > input.prevY) {
        cam.turnDown(0.25f * (input.y - input.prevY) * dt);
        input.prevY = input.y;
      }

      const auto x = static_cast<float>(input.x);
      const auto y = static_cast<float>(input.y);
      const auto w = static_cast<float>(window_->width());
      const auto h = static_cast<float>(window_->height());

      auto project = [&] {
        auto p = object_->worldTransform()[3];
        p = cam.transform() * p;
        p /= p[3];
        p *= 0.5f;
        p += 0.5f;
        p[0] *= w;
        p[1] *= h;
        return Vec3f{p[0], p[1], p[2]};
      };

      auto unproject = [&] {
        Vec4f p{x / w, y / h, 1.0f, 1.0f};
        p *= 2.0f;
        p -= 1.0f;
        p = invert(cam.transform()) * p;
        p /= p[3];
        return Vec3f{p[0], p[1], p[2]};
      };

      if (object_) {
        const auto l = object_->worldTransform()[3];
        const auto p = project();
        wcout << "\nobject: \n"
              << " loc.  (" << l[0] << ", " << l[1] << ", " << l[2] << ")\n"
              << " proj. (" << p[0] << ", " << p[1] << ", " << p[2] << ")\n";
      }

      const auto u = unproject();
      wcout << "\npointer: \n"
            << " loc.    (" << input.x << ", " << input.y << ")\n"
            << " unproj. (" << u[0] << "," << u[1] << "," << u[2] << ")\n";
    }

    if (camMode || !object_) {
      if (input.moveF)
        cam.moveForward(deltaM * dt);
      if (input.moveB)
        cam.moveBackward(deltaM * dt);
      if (input.moveL)
        cam.moveLeft(deltaM * dt);
      if (input.moveR)
        cam.moveRight(deltaM * dt);
      if (input.moveU)
        cam.moveUp(deltaM * dt);
      if (input.moveD)
        cam.moveDown(deltaM * dt);
      if (input.turnL)
        cam.turnLeft(deltaT * dt);
      if (input.turnR)
        cam.turnRight(deltaT * dt);
      if (input.turnU)
        cam.turnUp(deltaT * dt);
      if (input.turnD)
        cam.turnDown(deltaT * dt);
      if (input.zoomI)
        cam.zoomIn(deltaZ * dt);
      if (input.zoomO)
        cam.zoomOut(deltaZ * dt);
      if (input.place)
        cam.place({0.0f, 0.0f, 20.0f});
      if (input.point)
        cam.point({});
    } else {
      Vec3f t;
      Qnionf r(1.0f, {});
      if (input.moveF)
        t[2] += 10.0f;
      if (input.moveB)
        t[2] -= 10.0f;
      if (input.moveL)
        t[0] += 10.0f;
      if (input.moveR)
        t[0] -= 10.0f;
      if (input.moveU)
        t[1] += 10.0f;
      if (input.moveD)
        t[1] -= 10.0f;
      if (input.turnL)
        r *= rotateQY(6.0f * dt);
      if (input.turnR)
        r *= rotateQY(-6.0f * dt);
      if (input.turnU)
        r *= rotateQX(-6.0f * dt);
      if (input.turnD)
        r *= rotateQX(6.0f * dt);
      if (input.place)
        t = {0.0f, 0.0f, 0.0f};
      object_->transform() *= translate(t * dt) * rotate(r);
    }

    return fn ? fn(elapsedTime) : true;
  });

  window_->close();
  input = {};
}

void InteractiveTest::onMotion(int32_t x, int32_t y) {
  input.prevX = input.x;
  input.prevY = input.y;
  input.x = x;
  input.y = y;
}

void InteractiveTest::onButton(WS_NS::Button button, WS_NS::ButtonState state,
                               int32_t x, int32_t y) {

  const bool b = state == WS_NS::ButtonStatePressed;

  switch (button) {
  case WS_NS::ButtonLeft:
    input.primary = b;
    break;
  case WS_NS::ButtonRight:
    input.secondary = b;
    break;
  default:
    break;
  }

  input.prevX = input.x;
  input.prevY = input.y;
  input.x = x;
  input.y = y;
}

void InteractiveTest::onKey(WS_NS::KeyCode key, WS_NS::KeyState state,
                            [[maybe_unused]] WS_NS::KeyModMask modMask) {

  const bool b = state == WS_NS::KeyStatePressed;

  switch (key) {
  case WS_NS::KeyCodeW:
    input.moveF = b;
    break;
  case WS_NS::KeyCodeS:
    input.moveB = b;
    break;
  case WS_NS::KeyCodeA:
    input.moveL= b;
    break;
  case WS_NS::KeyCodeD:
    input.moveR = b;
    break;
  case WS_NS::KeyCodeR:
    input.moveU= b;
    break;
  case WS_NS::KeyCodeF:
    input.moveD = b;
    break;
  case WS_NS::KeyCodeLeft:
    input.turnL = b;
    break;
  case WS_NS::KeyCodeRight:
    input.turnR = b;
    break;
  case WS_NS::KeyCodeUp:
    input.turnU = b;
    break;
  case WS_NS::KeyCodeDown:
    input.turnD = b;
    break;
  case WS_NS::KeyCodeE:
    input.zoomI = b;
    break;
  case WS_NS::KeyCodeQ:
    input.zoomO = b;
    break;
  case WS_NS::KeyCodeReturn:
    input.place = b;
    break;
  case WS_NS::KeyCodeSpace:
    input.point = b;
    break;
  case WS_NS::KeyCodeM:
    input.mode = b;
    break;
  case WS_NS::KeyCodeComma:
    input.prev = b;
    break;
  case WS_NS::KeyCodeDot:
    input.next = b;
    break;
  case WS_NS::KeyCodeEsc:
    input.quit = b;
    break;
  default:
    break;
  }
}
