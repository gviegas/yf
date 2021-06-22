//
// SG
// InteractiveTest.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_INTERACTIVETEST_H
#define YF_SG_INTERACTIVETEST_H

#include <cstdint>

#include "yf/ws/WS.h"

#include "Test.h"
#include "View.h"
#include "Scene.h"
#include "Node.h"

TEST_NS_BEGIN

class InteractiveTest : public Test {
 public:
  struct Input {
    int32_t x, y;
    bool primary, secondary;
    bool moveF, moveB, moveL, moveR, moveU, moveD;
    bool turnL, turnR, turnU, turnD;
    bool zoomI, zoomO;
    bool place, point;
    bool mode, prev, next;
    bool quit;
  };

  InteractiveTest(std::wstring&& name, uint32_t width, uint32_t height);

  virtual Assertions run(const std::vector<std::string>& args) = 0;

  void setScene(SG_NS::Scene* scene);
  void setObject(SG_NS::Node* node);
  void update(SG_NS::Scene& scene, const SG_NS::View::UpdateFn& fn);
  Input& input();

 private:
  WS_NS::Window::Ptr window_;
  SG_NS::View view_;
  SG_NS::Scene* scene_;
  SG_NS::Node* object_;
  SG_NS::View::UpdateFn updateFn_;
  Input input_;

  void onMotion(int32_t, int32_t);
  void onButton(WS_NS::Button, WS_NS::ButtonState, int32_t, int32_t);
  void onKey(WS_NS::KeyCode, WS_NS::KeyState, WS_NS::KeyModMask);
};

TEST_NS_END

#endif // YF_SG_INTERACTIVETEST_H
