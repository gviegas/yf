//
// CG
// PassTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "Pass.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct PassTest : Test {
  PassTest() : Test(L"Pass") { }

  Assertions run(const vector<string>&) {
    struct Pass_ : Pass {
      Pass_(const vector<ColorAttach>* colors,
            const vector<ColorAttach>* resolves,
            const DepStenAttach* depthStencil)
        : Pass(colors, resolves, depthStencil) { }

      Target::Ptr makeTarget(Size2,
                             uint32_t,
                             const vector<AttachImg>*,
                             const vector<AttachImg>*,
                             const AttachImg*) { return nullptr; }
    };

    Assertions a;

    vector<ColorAttach> cols{
      {PxFormatBgra8Srgb, Samples4, LoadOpLoad, StoreOpDontCare}};
    vector<ColorAttach> resvs{
      {PxFormatBgra8Srgb, Samples1, LoadOpDontCare, StoreOpStore}};
    DepStenAttach depSten{PxFormatD16Unorm, Samples1,
                          LoadOpDontCare, StoreOpStore,
                          LoadOpDontCare, StoreOpDontCare};
    Pass_ p1(&cols, &resvs, &depSten);

    cols.push_back({PxFormatRgba8Unorm, Samples1, LoadOpLoad, StoreOpStore});
    cols.front().samples = Samples1;
    depSten.depStoreOp = StoreOpDontCare;
    Pass_ p2(&cols, nullptr, &depSten);

    Pass_ p3(&cols, nullptr, nullptr);

    Pass_ p4(nullptr, nullptr, &depSten);

    a.push_back({L"Pass(#one color, resolve and depth/stencil#)",
                 p1.colors_ != nullptr && p1.colors_->size() == 1 &&
                 p1.colors_->front().format == PxFormatBgra8Srgb &&
                 p1.colors_->front().samples == Samples4 &&
                 p1.colors_->front().loadOp == LoadOpLoad &&
                 p1.colors_->front().storeOp == StoreOpDontCare &&
                 p1.resolves_ != nullptr && p1.resolves_->size() == 1 &&
                 p1.resolves_->front().format == PxFormatBgra8Srgb &&
                 p1.resolves_->front().samples == Samples1 &&
                 p1.resolves_->front().loadOp == LoadOpDontCare &&
                 p1.resolves_->front().storeOp == StoreOpStore &&
                 p1.depthStencil_ != nullptr &&
                 p1.depthStencil_->format == PxFormatD16Unorm &&
                 p1.depthStencil_->samples == Samples1 &&
                 p1.depthStencil_->depLoadOp == LoadOpDontCare &&
                 p1.depthStencil_->depStoreOp == StoreOpStore &&
                 p1.depthStencil_->stenLoadOp == LoadOpDontCare &&
                 p1.depthStencil_->stenStoreOp == StoreOpDontCare});

    a.push_back({L"Pass(#two colors and depth/stencil, no resolve#)",
                 p2.colors_ != nullptr && p2.colors_->size() == 2 &&
                 p2.colors_->front().format == PxFormatBgra8Srgb &&
                 p2.colors_->front().samples == Samples1 &&
                 p2.colors_->front().loadOp == LoadOpLoad &&
                 p2.colors_->front().storeOp == StoreOpDontCare &&
                 p2.colors_->back().format == PxFormatRgba8Unorm &&
                 p2.colors_->back().samples == Samples1 &&
                 p2.colors_->back().loadOp == LoadOpLoad &&
                 p2.colors_->back().storeOp == StoreOpStore &&
                 p2.resolves_ == nullptr &&
                 p2.depthStencil_ != nullptr &&
                 p2.depthStencil_->format == PxFormatD16Unorm &&
                 p2.depthStencil_->samples == Samples1 &&
                 p2.depthStencil_->depLoadOp == LoadOpDontCare &&
                 p2.depthStencil_->depStoreOp == StoreOpDontCare &&
                 p2.depthStencil_->stenLoadOp == LoadOpDontCare &&
                 p2.depthStencil_->stenStoreOp == StoreOpDontCare});

    a.push_back({L"Pass(#color only#)",
                 p3.colors_ != nullptr && p3.colors_->size() == 2 &&
                 p3.colors_->front().format == PxFormatBgra8Srgb &&
                 p3.colors_->front().samples == Samples1 &&
                 p3.colors_->front().loadOp == LoadOpLoad &&
                 p3.colors_->front().storeOp == StoreOpDontCare &&
                 p3.colors_->back().format == PxFormatRgba8Unorm &&
                 p3.colors_->back().samples == Samples1 &&
                 p3.colors_->back().loadOp == LoadOpLoad &&
                 p3.colors_->back().storeOp == StoreOpStore &&
                 p3.resolves_ == nullptr &&
                 p3.depthStencil_ == nullptr});

    a.push_back({L"Pass(#depth/stencil only#)",
                 p4.colors_ == nullptr &&
                 p4.resolves_ == nullptr &&
                 p4.depthStencil_ != nullptr &&
                 p4.depthStencil_->format == PxFormatD16Unorm &&
                 p4.depthStencil_->samples == Samples1 &&
                 p4.depthStencil_->depLoadOp == LoadOpDontCare &&
                 p4.depthStencil_->depStoreOp == StoreOpDontCare &&
                 p4.depthStencil_->stenLoadOp == LoadOpDontCare &&
                 p4.depthStencil_->stenStoreOp == StoreOpDontCare});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::passTest() {
  static PassTest test;
  return &test;
}
