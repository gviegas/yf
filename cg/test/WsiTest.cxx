//
// CG
// WsiTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <thread>
#include <cassert>

#include "Test.h"
#include "Wsi.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct WsiTest : Test {
  WsiTest() : Test(L"Wsi") { }

  Assertions run(const vector<string>&) {
    class Wsi_ : public Wsi {
      WS_NS::Window* window_;
      vector<Image*> images_{3, nullptr};
     public:
      Wsi_(WS_NS::Window* window) : window_(window) { }
      Image* operator[](Index index) { return images_[index]; }
      const Image* operator[](Index index) const { return images_[index]; }
      Image* const* begin() { return images_.data(); }
      const Image* const* begin() const { return images_.data(); }
      Image* const* end() { return images_.data()+images_.size(); }
      const Image* const* end() const { return images_.data()+images_.size(); }
      const vector<Image*>& images() const { return images_; }
      uint32_t maxImages() const { return 1; }
      pair<Image*, Index> nextImage(bool) { return {nullptr, UINT32_MAX}; }
      void present(Index) { }
      WS_NS::Window& window() { return *window_; }
    };

    Assertions a;

    auto win = WS_NS::createWindow(240, 144, name_, WS_NS::Window::Resizable |
                                                    WS_NS::Window::Hidden);
    Wsi_ wsi(win.get());

    wsi.window().open();
    this_thread::sleep_for(chrono::seconds(1));
    wsi.window().close();
    this_thread::sleep_for(chrono::milliseconds(500));

    a.push_back({L"Wsi(...)", &wsi.window() == win.get()});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* wsiTest() {
  static WsiTest test;
  return &test;
}

TEST_NS_END
