//
// SG
// RenderTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "yf/ws/WS.h"

#include "Test.h"
#include "SG.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct RenderTest : Test {
  RenderTest() : Test(L"Render") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    auto win = WS_NS::createWindow(480, 300, name_);
    View view{win.get()};

    Mesh mesh1{Mesh::Gltf, L"tmp/cube.gltf"};
    Mesh mesh2{Mesh::Gltf, L"tmp/cube.gltf"};
    Texture tex{Texture::Png, L"tmp/cube.png"};
    Material matl;
    matl.pbrmr().colorTex = &tex;

    Model mdl1{mesh1, matl};
    Model mdl2{mesh2};
    mdl2.setMaterial(&matl);
    mdl1.transform() = translate(5.0f, 0.0f, 0.0f);
    mdl2.transform() = rotateZ(3.14159265359f * 0.5f);

    Scene scn1;
    scn1.insert(mdl1);
    Scene scn2;
    scn2.insert(mdl2);

    auto key = WS_NS::KeyCodeUnknown;

    WS_NS::KbDelegate deleg;
    deleg.key = [&](auto code, auto state, auto) {
      if (state == WS_NS::KeyStatePressed) {
        key = code;
        if (code == WS_NS::KeyCode1)
          view.swapScene(scn1);
        else if (code == WS_NS::KeyCode2)
          view.swapScene(scn2);
      }
    };
    WS_NS::setDelegate(deleg);

    view.loop(scn1, 30, [&](auto) { return key != WS_NS::KeyCodeEsc; });

    return {{L"Scene graph rendered", true}};
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* renderTest() {
  static RenderTest test;
  return &test;
}

TEST_NS_END
