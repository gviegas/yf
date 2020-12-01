//
// WS
// EventTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <thread>
#include <iostream>

#include "UnitTests.h"
#include "Window.h"
#include "Event.h"
#include "Keyboard.h"
#include "Pointer.h"

using namespace TEST_NS;
using namespace WS_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct EventTest : Test {
  EventTest() : Test(L"Event") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    auto& ev = Event::get();

    WdDelegate wdDeleg{};
    ev.setDelegate(wdDeleg);

    KbDelegate kbDeleg{
      [](Window* win) { wcout << "kb enter: " << win << endl; },
      [](Window* win) { wcout << "kb leave: " << win << endl; },
      [](KeyCode c, KeyState s, KeyModMask m) {
        wcout << "kb key: " << c << ", " << s << ", " << hex << m << dec
              << endl; }
    };
    ev.setDelegate(kbDeleg);

    PtDelegate ptDeleg{
      [](Window* win, int32_t x, int32_t y) {
        wcout << "pt enter: " << win << ", " << x << ", " << y << endl; },
      [](Window* win) { wcout << "pt leave: " << win << endl; },
      [](int32_t x, int32_t y) {
        wcout << "pt motion: " << x << ", " << y << endl; },
      [](Button b, BtnState s, int32_t x, int32_t y) {
        wcout << "pt button: " << b << ", " << s << "," << x << ", " << y
              << endl; }
    };
    ev.setDelegate(ptDeleg);

    auto win1 = Window::make(300, 200, L"Window 1");
    auto win2 = Window::make(100, 300, L"Window 2");

    auto tm = chrono::system_clock::now() + chrono::seconds(30);
    while (tm > chrono::system_clock::now()) {
      ev.dispatch();
      this_thread::sleep_for(chrono::milliseconds(10));
    }

    a.push_back({L"Event::get()", true});
    a.push_back({L"ev.setDelegate(WdDelegate)", true});
    a.push_back({L"ev.setDelegate(KbDelegate)", true});
    a.push_back({L"ev.setDelegate(PtDelegate)", true});
    a.push_back({L"ev.dispatch()", true});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::eventTest() {
  static EventTest test;
  return &test;
}
