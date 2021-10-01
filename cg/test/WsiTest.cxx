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
#include "Device.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct WsiTest : Test {
  WsiTest() : Test(L"Wsi") { }

  Assertions run(const vector<string>&) {
    class Wsi_ : public Wsi {
      WS_NS::Window* window_;
      vector<Image*> images_;
     public:
      Wsi_(WS_NS::Window* window) : window_(window), images_() {
        static auto i1 = device().image(PxFormatR8Unorm, {16}, 1, 1, Samples1);
        static auto i2 = device().image(PxFormatR8Unorm, {16}, 2, 1, Samples1);
        static auto i3 = device().image(PxFormatR8Unorm, {16}, 3, 1, Samples1);
        static auto i4 = device().image(PxFormatR8Unorm, {16}, 4, 1, Samples1);
        images_.push_back(i1.get());
        images_.push_back(i3.get());
        images_.push_back(i4.get());
        images_.push_back(i2.get());
      }
      Image* operator[](Index index) { return images_[index]; }
      const Image* operator[](Index index) const { return images_[index]; }
      Image* const* begin() { return images_.data(); }
      const Image* const* begin() const { return images_.data(); }
      Image* const* end() { return images_.data()+images_.size(); }
      const Image* const* end() const { return images_.data()+images_.size(); }
      uint32_t size() const { return images_.size(); }
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

    bool chk = true;
    Wsi::Index ix = 0;
    const uint32_t lays[]{1, 3, 4, 2};
    for (auto& img : wsi) {
      if (img != wsi[ix] || img->layers() != lays[ix++]) {
        chk = false;
        break;
      }
    }

    a.push_back({L"operator[], begin(), end()", chk});

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
