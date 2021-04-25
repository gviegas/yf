//
// WS
// EventTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <thread>
#include <iostream>

#include "Test.h"
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

    KeyCode key = KeyCodeUnknown;

    onWdClose([](Window* win) { wcout << "wd close: " << win << endl; });
    onWdResize([](Window* win, uint32_t w, uint32_t h) {
        wcout << "wd resize: " << win << ", " << w << ", " << h << endl;
    });

    onKbEnter([](Window* win) { wcout << "kb enter: " << win << endl; });
    onKbLeave([](Window* win) { wcout << "kb leave: " << win << endl; });
    onKbKey([&key](KeyCode c, KeyState s, KeyModMask m) {
      wcout << "kb key: " << c << ", " << s << ", " << hex << m << dec << endl;
      key = c;
    });

    onPtEnter([](Window* win, int32_t x, int32_t y) {
      wcout << "pt enter: " << win << ", " << x << ", " << y << endl;
    });
    onPtLeave([](Window* win) { wcout << "pt leave: " << win << endl; });
    onPtMotion([](int32_t x, int32_t y) {
      wcout << "pt motion: " << x << ", " << y << endl;
    });
    onPtButton([](Button b, ButtonState s, int32_t x, int32_t y) {
      wcout << "pt button: " << b << ", " << s << "," << x << ", " << y << endl;
    });

    auto win1 = createWindow(300, 200, L"Window 1");
    auto win2 = createWindow(100, 300, L"Window 2");

    auto tm = chrono::system_clock::now() + chrono::minutes(1);
    while (tm > chrono::system_clock::now()) {
      dispatch();
      this_thread::sleep_for(chrono::milliseconds(10));

      if (key == KeyCodeEsc)
        break;

      if (key == KeyCode1)
        onWdClose({});
      else if (key == KeyCode2)
        onWdClose([](Window* win) {
          wcout << "new wd close: " << win << endl;
        });
    }

    a.push_back({L"setDelegate(WdDelegate)", true});
    a.push_back({L"setDelegate(KbDelegate)", true});
    a.push_back({L"setDelegate(PtDelegate)", true});
    a.push_back({L"dispatch()", true});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* eventTest() {
  static EventTest test;
  return &test;
}

TEST_NS_END
