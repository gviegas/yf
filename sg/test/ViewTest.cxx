//
// SG
// ViewTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <iostream>

#include "UnitTests.h"
#include "View.h"
#include "Scene.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ViewTest : Test {
  ViewTest() : Test(L"View") { }

  Assertions run(const vector<string>&) {
    auto win = WS_NS::Window::make(400, 240, L"ViewTest");
    Scene scn;

    View view(win.get());

    for (uint32_t fps : {24, 30, 60}) {
      chrono::nanoseconds elapsed{};
      wcout << "\n<loop() [" << fps << " FPS]>\n";

      view.loop(scn, fps, [&](auto dt) {
        elapsed += dt;
        wcout << "(t) " << (double)dt.count()/1e9 << "\n";
        return elapsed.count() < 1'000'000'000;
      });
    }

    return {{L"View()", true}};
  }
};

INTERNAL_NS_END

Test* TEST_NS::viewTest() {
  static ViewTest test;
  return &test;
}
