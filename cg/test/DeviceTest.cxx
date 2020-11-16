//
// CG
// DeviceTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "Device.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct DeviceTest : Test {
  DeviceTest() : Test(L"Device") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    auto& dev = Device::get();

    auto buf = dev.makeBuffer(1<<28);

    auto img1 = dev.makeImage(PxFormatRgba8Unorm, {600, 600}, 4, 1, Samples1);
    auto img2 = dev.makeImage(PxFormatD16Unorm, {600, 600}, 1, 1, Samples1);

    auto shd = dev.makeShader(StageFragment, L"tmp/frag");

    DcEntries ents{{2, {DcTypeImage, 4}},
                   {0, {DcTypeUniform, 1}},
                   {1, {DcTypeStorage, 1}},
                   {4, {DcTypeImgSampler, 16}}};
    auto dtb = dev.makeDcTable(ents);
    bool dtbAllocs[4];
    dtbAllocs[0] = dtb->allocations() == 0;
    dtb->allocate(16);
    dtbAllocs[1] = dtb->allocations() == 16;
    dtb->allocate(0);
    dtbAllocs[2] = dtb->allocations() == 0;
    dtb->allocate(1);
    dtbAllocs[3] = dtb->allocations() == 1;

    vector<ColorAttach> clrs{{PxFormatRgba8Unorm, Samples1, LoadOpLoad,
                              StoreOpStore}};
    DepStenAttach depSten{PxFormatD16Unorm, Samples1, LoadOpLoad,
                          StoreOpStore, LoadOpDontCare, StoreOpDontCare};
    auto pass = dev.makePass(&clrs, nullptr, &depSten);

    vector<AttachImg> clrImgs{{img1.get(), 0, 0}};
    AttachImg depImg{img2.get(), 0, 0};
    auto targ = pass->makeTarget({600, 600}, 1, &clrImgs, nullptr, &depImg);

    auto cpShd = dev.makeShader(StageCompute, L"tmp/comp");
    CpState::Config cpConfig = {cpShd.get(), vector<DcTable*>{}};
    auto cpState = dev.makeState(cpConfig);

    a.push_back({L"Device::get()", true});
    a.push_back({L"dev.makeBuffer(...)", buf != nullptr});
    a.push_back({L"dev.makeImage(...) (1)", img1 != nullptr});
    a.push_back({L"dev.makeImage(...) (2)", img2 != nullptr});
    a.push_back({L"dev.makeShader(...)", shd != nullptr});
    a.push_back({L"dev.makeDcTable(...)", dtb != nullptr});
    a.push_back({L"dtb.allocate(...) (1)", dtbAllocs[0]});
    a.push_back({L"dtb.allocate(...) (2)", dtbAllocs[1]});
    a.push_back({L"dtb.allocate(...) (3)", dtbAllocs[2]});
    a.push_back({L"dtb.allocate(...) (4)", dtbAllocs[3]});
    a.push_back({L"dev.makePass(...)", pass != nullptr});
    a.push_back({L"pass.makeTarget(...)", targ != nullptr});
    a.push_back({L"dev.makeState(Cp)", cpState != nullptr});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::deviceTest() {
  static DeviceTest test;
  return &test;
}
