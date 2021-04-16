//
// WS
// WindowTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <thread>

#include "Test.h"
#include "Window.h"

using namespace TEST_NS;
using namespace WS_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct WindowTest : Test {
  WindowTest() : Test(L"Window") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    Window::appId = L"window.test.id";

    auto win1 = createWindow(800, 600, L"Window 1");
    auto win2 = createWindow(480, 480, L"Window 2");
    auto win3 = createWindow(240, 320, L"Window 3",
                             Window::Resizable | Window::Hidden);

    this_thread::sleep_for(chrono::milliseconds(1250));
    win3->open();
    this_thread::sleep_for(chrono::milliseconds(1250));
    win1->close();
    this_thread::sleep_for(chrono::milliseconds(1250));
    win2->close();
    win3->close();

    a.push_back({L"createWindow(...) (1)", true});
    a.push_back({L"createWindow(...) (2)", true});
    a.push_back({L"createWindow(...) (3)", true});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* windowTest() {
  static WindowTest test;
  return &test;
}

TEST_NS_END
