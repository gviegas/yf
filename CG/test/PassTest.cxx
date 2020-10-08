//
// yf
// PassTest.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "CGPass.h"
#include "Defs.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct PassTest : Test {
  PassTest() : Test(L"CGPass") {}

  Assertions run(const vector<string>& args) {
    struct Pass : CGPass {
      Pass(const vector<CGColorAttach>* colors,
           const vector<CGColorAttach>* resolves,
           const CGDepStenAttach* depthStencil)
           : CGPass(colors, resolves, depthStencil) {}

      TargetPtr makeTarget(CGSize2,
                           uint32_t,
                           const vector<CGAttachImg>*,
                           const vector<CGAttachImg>*,
                           const CGAttachImg*) { return nullptr; }
    };

    Assertions a;

    vector<CGColorAttach> cols{{CGPxFormatBgra8Srgb,
                                CGSamples4,
                                CGLoadOpLoad,
                                CGStoreOpDontCare}};
    vector<CGColorAttach> resvs{{CGPxFormatBgra8Srgb,
                                 CGSamples1,
                                 CGLoadOpDontCare,
                                 CGStoreOpStore}};
    CGDepStenAttach depSten{CGPxFormatD16S8Unorm,
                            CGSamples1,
                            CGLoadOpDontCare,
                            CGStoreOpStore,
                            CGLoadOpDontCare,
                            CGStoreOpDontCare};
    Pass p1(&cols, &resvs, &depSten);

    cols.push_back({CGPxFormatRgba8Unorm,
                    CGSamples1,
                    CGLoadOpLoad,
                    CGStoreOpStore});
    cols.front().samples = CGSamples1;
    depSten.depStoreOp = CGStoreOpDontCare;
    Pass p2(&cols, nullptr, &depSten);

    Pass p3(&cols, nullptr, nullptr);

    Pass p4(nullptr, nullptr, &depSten);

    a.push_back({L"CGPass(#one color, resolve and depth/stencil#)",
                 p1.colors != nullptr && p1.colors->size() == 1 &&
                 p1.colors->front().format == CGPxFormatBgra8Srgb &&
                 p1.colors->front().samples == CGSamples4 &&
                 p1.colors->front().loadOp == CGLoadOpLoad &&
                 p1.colors->front().storeOp == CGStoreOpDontCare &&
                 p1.resolves != nullptr && p1.resolves->size() == 1 &&
                 p1.resolves->front().format == CGPxFormatBgra8Srgb &&
                 p1.resolves->front().samples == CGSamples1 &&
                 p1.resolves->front().loadOp == CGLoadOpDontCare &&
                 p1.resolves->front().storeOp == CGStoreOpStore &&
                 p1.depthStencil != nullptr &&
                 p1.depthStencil->format == CGPxFormatD16S8Unorm &&
                 p1.depthStencil->samples == CGSamples1 &&
                 p1.depthStencil->depLoadOp == CGLoadOpDontCare &&
                 p1.depthStencil->depStoreOp == CGStoreOpStore &&
                 p1.depthStencil->stenLoadOp == CGLoadOpDontCare &&
                 p1.depthStencil->stenStoreOp == CGStoreOpDontCare});

    a.push_back({L"CGPass(#two colors and depth/stencil, no resolve#)",
                 p2.colors != nullptr && p2.colors->size() == 2 &&
                 p2.colors->front().format == CGPxFormatBgra8Srgb &&
                 p2.colors->front().samples == CGSamples1 &&
                 p2.colors->front().loadOp == CGLoadOpLoad &&
                 p2.colors->front().storeOp == CGStoreOpDontCare &&
                 p2.colors->back().format == CGPxFormatRgba8Unorm &&
                 p2.colors->back().samples == CGSamples1 &&
                 p2.colors->back().loadOp == CGLoadOpLoad &&
                 p2.colors->back().storeOp == CGStoreOpStore &&
                 p2.resolves == nullptr &&
                 p2.depthStencil != nullptr &&
                 p2.depthStencil->format == CGPxFormatD16S8Unorm &&
                 p2.depthStencil->samples == CGSamples1 &&
                 p2.depthStencil->depLoadOp == CGLoadOpDontCare &&
                 p2.depthStencil->depStoreOp == CGStoreOpDontCare &&
                 p2.depthStencil->stenLoadOp == CGLoadOpDontCare &&
                 p2.depthStencil->stenStoreOp == CGStoreOpDontCare});

    a.push_back({L"CGPass(#color only#)",
                 p3.colors != nullptr && p3.colors->size() == 2 &&
                 p3.colors->front().format == CGPxFormatBgra8Srgb &&
                 p3.colors->front().samples == CGSamples1 &&
                 p3.colors->front().loadOp == CGLoadOpLoad &&
                 p3.colors->front().storeOp == CGStoreOpDontCare &&
                 p3.colors->back().format == CGPxFormatRgba8Unorm &&
                 p3.colors->back().samples == CGSamples1 &&
                 p3.colors->back().loadOp == CGLoadOpLoad &&
                 p3.colors->back().storeOp == CGStoreOpStore &&
                 p3.resolves == nullptr &&
                 p3.depthStencil == nullptr});

    a.push_back({L"CGPass(#depth/stencil only#)",
                 p4.colors == nullptr &&
                 p4.resolves == nullptr &&
                 p4.depthStencil != nullptr &&
                 p4.depthStencil->format == CGPxFormatD16S8Unorm &&
                 p4.depthStencil->samples == CGSamples1 &&
                 p4.depthStencil->depLoadOp == CGLoadOpDontCare &&
                 p4.depthStencil->depStoreOp == CGStoreOpDontCare &&
                 p4.depthStencil->stenLoadOp == CGLoadOpDontCare &&
                 p4.depthStencil->stenStoreOp == CGStoreOpDontCare});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::passTest() {
  static PassTest test;
  return &test;
}
