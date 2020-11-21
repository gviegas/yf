//
// WS
// WindowTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <thread>

#include "UnitTests.h"
#include "Window.h"

using namespace TEST_NS;
using namespace WS_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct WindowTest : Test {
  WindowTest() : Test(L"Window") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    auto win1 = Window::make(800, 600, L"Window 1");
    auto win2 = Window::make(480, 480, L"Window 2");
    auto win3 = Window::make(240, 320, L"Window 3", Window::Resizable |
                                                    Window::Hidden);

    this_thread::sleep_for(chrono::milliseconds(1250));
    win3->open();
    this_thread::sleep_for(chrono::milliseconds(1250));
    win1->close();
    this_thread::sleep_for(chrono::milliseconds(1250));
    win2->close();
    win3->close();

    a.push_back({L"Window::make(...) (1)", true});
    a.push_back({L"Window::make(...) (2)", true});
    a.push_back({L"Window::make(...) (3)", true});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::windowTest() {
  static WindowTest test;
  return &test;
}
