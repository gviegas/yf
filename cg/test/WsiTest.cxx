//
// CG
// WsiTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <thread>

#include "UnitTests.h"
#include "Wsi.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct WsiTest : Test {
  WsiTest() : Test(L"Wsi") { }

  Assertions run(const vector<string>&) {
    struct Wsi_ : Wsi {
      Wsi_(WS_NS::Window* win) : Wsi(win) { }
      const vector<Image*>& images() const { abort(); }
      Image* nextImage() { return nullptr; }
      void present(Image*) { }
    };

    Assertions a;

    auto win = WS_NS::Window::make(240, 144, L"wsi test",
                                   WS_NS::Window::Resizable |
                                   WS_NS::Window::Hidden);
    Wsi_ wsi(win.get());

    wsi.window_->open();
    this_thread::sleep_for(chrono::seconds(1));
    wsi.window_->close();
    this_thread::sleep_for(chrono::milliseconds(500));

    a.push_back({L"Wsi(...)", wsi.window_ == win.get()});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::wsiTest() {
  static WsiTest test;
  return &test;
}
