//
// SG
// InteractiveTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "InteractiveTest.h"
#include "Camera.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

constexpr float deltaM = 0.215f;
constexpr float deltaT = 0.025f;
constexpr float deltaZ = 0.035f;

InteractiveTest::Input InteractiveTest::input;

InteractiveTest::InteractiveTest(wstring&& name, uint32_t width,
                                 uint32_t height)
  : Test(move(name)), window_(WS_NS::createWindow(width, height, name)),
    view_(window_.get()), scene_{}, object_{} { }

void InteractiveTest::setScene(Scene* scene) {
  scene_ = scene;
}

void InteractiveTest::setObject(Node* node) {
  object_ = node;
}

void InteractiveTest::update(Scene& scene, const View::UpdateFn& fn) {
  scene_ = &scene;

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

    if (camMode || !object_) {
      if (input.moveF)
        cam.moveForward(deltaM);
      if (input.moveB)
        cam.moveBackward(deltaM);
      if (input.moveL)
        cam.moveLeft(deltaM);
      if (input.moveR)
        cam.moveRight(deltaM);
      if (input.moveU)
        cam.moveUp(deltaM);
      if (input.moveD)
        cam.moveDown(deltaM);
      if (input.turnL)
        cam.turnLeft(deltaT);
      if (input.turnR)
        cam.turnRight(deltaT);
      if (input.turnU)
        cam.turnUp(deltaT);
      if (input.turnD)
        cam.turnDown(deltaT);
      if (input.zoomI)
        cam.zoomIn(deltaZ);
      if (input.zoomO)
        cam.zoomOut(deltaZ);
      if (input.place)
        cam.place({0.0f, 0.0f, 20.0f});
      if (input.point)
        cam.point({});
    } else {
      Vec3f t;
      Qnionf r(1.0f, {});
      if (input.moveF)
        t[2] += 0.1f;
      if (input.moveB)
        t[2] -= 0.1f;
      if (input.moveL)
        t[0] += 0.1f;
      if (input.moveR)
        t[0] -= 0.1f;
      if (input.moveU)
        t[1] += 0.1f;
      if (input.moveD)
        t[1] -= 0.1f;
      if (input.turnL)
        r *= rotateQY(0.1f);
      if (input.turnR)
        r *= rotateQY(-0.1f);
      if (input.turnU)
        r *= rotateQX(-0.1f);
      if (input.turnD)
        r *= rotateQX(0.1f);
      if (input.place)
        t = {0.0f, 0.0f, 0.0f};
      object_->transform() *= translate(t) * rotate(r);
    }

    return fn ? fn(elapsedTime) : true;
  });

  input = {};
}

void InteractiveTest::onMotion(int32_t x, int32_t y) {
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
