//
// CG
// PassTest.cxx
//
// Copyright © 2020-2023 Gustavo C. Viegas.
//

#include "Test.h"
#include "Pass.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct PassTest : Test {
  PassTest() : Test(L"Pass") { }

  Assertions run(const vector<string>&) {
    class Pass_ : public Pass {
      vector<AttachDesc>* colors_;
      vector<AttachDesc>* resolves_;
      AttachDesc* depthStencil_;
     public:
      Pass_(const vector<AttachDesc>* colors,
            const vector<AttachDesc>* resolves,
            const AttachDesc* depthStencil)
        : colors_(colors ? new auto(*colors) : nullptr),
          resolves_(resolves ? new auto(*resolves) : nullptr),
          depthStencil_(depthStencil ? new auto(*depthStencil) : nullptr) { }

      ~Pass_() {
        delete colors_;
        delete resolves_;
        delete depthStencil_;
      }

      Target::Ptr target(Size2, uint32_t, const vector<AttachImg>*,
                         const vector<AttachImg>*, const AttachImg*)
                         { return nullptr; }

      const vector<AttachDesc>* colors() const { return colors_; }
      const vector<AttachDesc>* resolves() const { return resolves_; }
      const AttachDesc* depthStencil() const { return depthStencil_; }
    };

    Assertions a;

    vector<AttachDesc> clrs{{PxFormatBgra8Srgb, Samples4}};
    vector<AttachDesc> resvs{{PxFormatBgra8Srgb, Samples1}};
    AttachDesc depSten{PxFormatD16Unorm, Samples1};
    Pass_ p1(&clrs, &resvs, &depSten);

    clrs.push_back({PxFormatRgba8Unorm, Samples1});
    clrs.front().samples = Samples1;
    depSten.format = PxFormatD24UnormS8;
    Pass_ p2(&clrs, nullptr, &depSten);

    Pass_ p3(&clrs, nullptr, nullptr);

    Pass_ p4(nullptr, nullptr, &depSten);

    a.push_back({L"Pass(#one color, resolve and depth/stencil#)",
                 p1.colors() != nullptr && p1.colors()->size() == 1 &&
                 p1.colors()->front().format == PxFormatBgra8Srgb &&
                 p1.colors()->front().samples == Samples4 &&
                 p1.resolves() != nullptr && p1.resolves()->size() == 1 &&
                 p1.resolves()->front().format == PxFormatBgra8Srgb &&
                 p1.resolves()->front().samples == Samples1 &&
                 p1.depthStencil() != nullptr &&
                 p1.depthStencil()->format == PxFormatD16Unorm &&
                 p1.depthStencil()->samples == Samples1});

    a.push_back({L"Pass(#two colors and depth/stencil, no resolve#)",
                 p2.colors() != nullptr && p2.colors()->size() == 2 &&
                 p2.colors()->front().format == PxFormatBgra8Srgb &&
                 p2.colors()->front().samples == Samples1 &&
                 p2.colors()->back().format == PxFormatRgba8Unorm &&
                 p2.colors()->back().samples == Samples1 &&
                 p2.resolves() == nullptr &&
                 p2.depthStencil() != nullptr &&
                 p2.depthStencil()->format == PxFormatD24UnormS8 &&
                 p2.depthStencil()->samples == Samples1});

    a.push_back({L"Pass(#color only#)",
                 p3.colors() != nullptr && p3.colors()->size() == 2 &&
                 p3.colors()->front().format == PxFormatBgra8Srgb &&
                 p3.colors()->front().samples == Samples1 &&
                 p3.colors()->back().format == PxFormatRgba8Unorm &&
                 p3.colors()->back().samples == Samples1 &&
                 p3.resolves() == nullptr &&
                 p3.depthStencil() == nullptr});

    a.push_back({L"Pass(#depth/stencil only#)",
                 p4.colors() == nullptr &&
                 p4.resolves() == nullptr &&
                 p4.depthStencil() != nullptr &&
                 p4.depthStencil()->format == PxFormatD24UnormS8 &&
                 p4.depthStencil()->samples == Samples1});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* passTest() {
  static PassTest test;
  return &test;
}

TEST_NS_END
