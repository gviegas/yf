//
// WS
// EventTest.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#include <thread>
#include <iostream>

#include "Test.h"
#include "Window.h"
#include "Event.h"
#include "Keyboard.h"
#include "Pointer.h"
#include "Delegate.h"

using namespace TEST_NS;
using namespace WS_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct EventTest : Test {
  EventTest() : Test(L"Event") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    KeyCode key = KeyCodeUnknown;
    uint64_t count = 0;

    onWdClose([&count](Window* win) {
      wcout << "wd close: " << win << endl;
      count++;
    });
    onWdResize([&count](Window* win, uint32_t w, uint32_t h) {
      wcout << "wd resize: " << win << ", " << w << ", " << h << endl;
      count++;
    });

    onKbEnter([&count](Window* win) {
      wcout << "kb enter: " << win << endl;
      count++;
    });
    onKbLeave([&count](Window* win) {
      wcout << "kb leave: " << win << endl;
      count++;
    });
    onKbKey([&key, &count](KeyCode c, KeyState s, KeyModMask m) {
      wcout << "kb key: " << c << ", " << s << ", " << hex << m << dec << endl;
      key = c;
      count++;
    });

    onPtEnter([&count](Window* win, int32_t x, int32_t y) {
      wcout << "pt enter: " << win << ", " << x << ", " << y << endl;
      count++;
    });
    onPtLeave([&count](Window* win) {
      wcout << "pt leave: " << win << endl;
      count++;
    });
    onPtMotion([&count](int32_t x, int32_t y) {
      wcout << "pt motion: " << x << ", " << y << endl;
      count++;
    });
    onPtButton([&count](Button b, ButtonState s, int32_t x, int32_t y) {
      wcout << "pt button: " << b << ", " << s << "," << x << ", " << y << endl;
      count++;
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
    onWdClose([](Window*){});

    a.push_back({L"onWdClose(<function>)",
                delegate().mask_ & Delegate::WdClose && delegate().wdClose_});
    a.push_back({L"onWdResize(<function>)",
                delegate().mask_ & Delegate::WdResize && delegate().wdResize_});

    a.push_back({L"onKbEnter(<function>)",
                delegate().mask_ & Delegate::KbEnter && delegate().kbEnter_});
    a.push_back({L"onKbLeave(<function>)",
                delegate().mask_ & Delegate::KbLeave && delegate().kbLeave_});
    a.push_back({L"onKbKey(<function>)",
                delegate().mask_ & Delegate::KbKey && delegate().kbKey_});

    a.push_back({L"onPtEnter(<function>)",
                delegate().mask_ & Delegate::PtEnter && delegate().ptEnter_});
    a.push_back({L"onPtLeave(<function>)",
                delegate().mask_ & Delegate::PtLeave && delegate().ptLeave_});
    a.push_back({L"onPtMotion(<function>)",
                delegate().mask_ & Delegate::PtMotion && delegate().ptMotion_});
    a.push_back({L"onPtButton(<function>)",
                delegate().mask_ & Delegate::PtButton && delegate().ptButton_});

    onWdClose({});
    onWdResize({});
    onKbEnter({});
    onKbLeave({});
    onKbKey({});
    onPtEnter({});
    onPtLeave({});
    onPtMotion({});
    onPtButton({});

    a.push_back({L"onWdClose({})",
                !(delegate().mask_ & Delegate::WdClose) && !delegate().wdClose_});
    a.push_back({L"onWdResize({})",
                !(delegate().mask_ & Delegate::WdResize) && !delegate().wdResize_});

    a.push_back({L"onKbEnter({})",
                !(delegate().mask_ & Delegate::KbEnter) && !delegate().kbEnter_});
    a.push_back({L"onKbLeave({})",
                !(delegate().mask_ & Delegate::KbLeave) && !delegate().kbLeave_});
    a.push_back({L"onKbKey({})",
                !(delegate().mask_ & Delegate::KbKey) && !delegate().kbKey_});

    a.push_back({L"onPtEnter({})",
                !(delegate().mask_ & Delegate::PtEnter) && !delegate().ptEnter_});
    a.push_back({L"onPtLeave({})",
                !(delegate().mask_ & Delegate::PtLeave) && !delegate().ptLeave_});
    a.push_back({L"onPtMotion({})",
                !(delegate().mask_ & Delegate::PtMotion) && !delegate().ptMotion_});
    a.push_back({L"onPtButton({})",
                !(delegate().mask_ & Delegate::PtButton) && !delegate().ptButton_});

    // NOTE: Failure here might not be `dispatch()`'s fault
    a.push_back({L"dispatch()", count});
    wcout << "dispatched " << count << (count > 1 ? " events" : " event") << endl;

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
