//
// SG
// CameraTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "InteractiveTest.h"
#include "SG.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct CameraTest : InteractiveTest {
  CameraTest() : InteractiveTest(L"Camera", 640, 480) { }

  Assertions run(const vector<string>&) {
    Camera cam{{0.0f, 0.0f, 10.0f}, {}, 640.0f / 480.0f};

    Mesh mesh("test/data/cube4.glb");
    Model mdl(mesh);

    Scene scn{};
    scn.camera() = cam;
    scn.color() = {0.5f, 0.5f, 0.5f, 1.0f};
    scn.insert(mdl);

    setObject(&mdl);
    update(scn, {});

    return {{L"Camera()", true}};
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* cameraTest() {
  static CameraTest test;
  return &test;
}

TEST_NS_END
