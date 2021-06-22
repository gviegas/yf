//
// SG
// InteractiveTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "InteractiveTest.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

InteractiveTest::InteractiveTest(wstring&& name, uint32_t width,
                                 uint32_t height)
  : Test(move(name)), window_(WS_NS::createWindow(width, height, name)),
    view_(window_.get()), scene_{}, object_{}, updateFn_{}, input_{} { }

void InteractiveTest::setScene(Scene* scene) {
  scene_ = scene;
}

void InteractiveTest::setObject(Node* node) {
  object_ = node;
}

void InteractiveTest::update(Scene& scene, const View::UpdateFn& fn) {
  scene_ = &scene;
  updateFn_ = fn;

  view_.loop(scene, 60, [&](auto elapsedTime) {
    return fn(elapsedTime);
  });
}

InteractiveTest::Input& InteractiveTest::input() {
  return input_;
}


void InteractiveTest::onMotion(int32_t x, int32_t y) {
  input_.x = x;
  input_.y = y;
}

void InteractiveTest::onButton(WS_NS::Button button, WS_NS::ButtonState state,
                               int32_t x, int32_t y) {

  const bool b = state == WS_NS::ButtonStatePressed;

  switch (button) {
  case WS_NS::ButtonLeft:
    input_.primary = b;
    break;
  case WS_NS::ButtonRight:
    input_.secondary = b;
    break;
  default:
    break;
  }

  input_.x = x;
  input_.y = y;
}

void InteractiveTest::onKey(WS_NS::KeyCode key, WS_NS::KeyState state,
                            [[maybe_unused]] WS_NS::KeyModMask modMask) {

  const bool b = state == WS_NS::KeyStatePressed;

  switch (key) {
  case WS_NS::KeyCodeW:
    input_.moveF = b;
    break;
  case WS_NS::KeyCodeS:
    input_.moveB = b;
    break;
  case WS_NS::KeyCodeA:
    input_.moveL= b;
    break;
  case WS_NS::KeyCodeD:
    input_.moveR = b;
    break;
  case WS_NS::KeyCodeR:
    input_.moveU= b;
    break;
  case WS_NS::KeyCodeF:
    input_.moveD = b;
    break;
  case WS_NS::KeyCodeLeft:
    input_.turnL = b;
    break;
  case WS_NS::KeyCodeRight:
    input_.turnR = b;
    break;
  case WS_NS::KeyCodeUp:
    input_.turnU = b;
    break;
  case WS_NS::KeyCodeDown:
    input_.turnD = b;
    break;
  case WS_NS::KeyCodeE:
    input_.zoomI = b;
    break;
  case WS_NS::KeyCodeQ:
    input_.zoomO = b;
    break;
  case WS_NS::KeyCodeReturn:
    input_.place = b;
    break;
  case WS_NS::KeyCodeSpace:
    input_.point = b;
    break;
  case WS_NS::KeyCodeM:
    input_.mode = b;
    break;
  case WS_NS::KeyCodeComma:
    input_.prev = b;
    break;
  case WS_NS::KeyCodeDot:
    input_.next = b;
    break;
  case WS_NS::KeyCodeEsc:
    input_.quit = b;
    break;
  default:
    break;
  }
}
