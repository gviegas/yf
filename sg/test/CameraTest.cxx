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

    Mesh mesh("tmp/cube.glb");
    Material matl;
    matl.pbrmr() = {make_unique<Texture>("tmp/cube.png"),
                    {1.0f, 1.0f, 1.0f, 1.0f}, {}, 1.0f, 1.0f};
    Model mdl{};
    mdl.setMesh(&mesh);
    mdl.setMaterial(&matl);

    Scene scn{};
    scn.camera() = cam;
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
