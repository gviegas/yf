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

  struct Input {
    bool moveF, moveB, moveL, moveR, moveU, moveD;
    bool turnL, turnR, turnU, turnD;
    bool zoomI, zoomO;
    bool place, point;
    bool swapScn;
    bool quit;
  };

  static Input input;
  static constexpr float deltaM = 0.165f;
  static constexpr float deltaT = 0.025f;
  static constexpr float deltaZ = 0.035f;

  static void onKey(WS_NS::KeyCode key, WS_NS::KeyState state,
                    [[maybe_unused]] WS_NS::KeyModMask modMask) {

    const bool b = state == WS_NS::KeyStatePressed;

    switch (key) {
    case WS_NS::KeyCodeW:
      input.moveF = b;
      break;
    case WS_NS::KeyCodeS:
      input.moveB = b;
      break;
    case WS_NS::KeyCodeA:
      input.moveL= b;
      break;
    case WS_NS::KeyCodeD:
      input.moveR = b;
      break;
    case WS_NS::KeyCodeR:
      input.moveU= b;
      break;
    case WS_NS::KeyCodeF:
      input.moveD = b;
      break;
    case WS_NS::KeyCodeLeft:
      input.turnL = b;
      break;
    case WS_NS::KeyCodeRight:
      input.turnR = b;
      break;
    case WS_NS::KeyCodeUp:
      input.turnU = b;
      break;
    case WS_NS::KeyCodeDown:
      input.turnD = b;
      break;
    case WS_NS::KeyCodeE:
      input.zoomI = b;
      break;
    case WS_NS::KeyCodeQ:
      input.zoomO = b;
      break;
    case WS_NS::KeyCodeReturn:
      input.place = b;
      break;
    case WS_NS::KeyCodeSpace:
      input.point = b;
      break;
    case WS_NS::KeyCodeTab:
      input.swapScn = b;
      break;
    case WS_NS::KeyCodeEsc:
      input.quit = true;
      break;
    default:
      break;
    }
  }

  Assertions run(const vector<string>&) {
    Assertions a;

    auto win = WS_NS::createWindow(800, 480, name_);
    View view{win.get()};

    Mesh mesh1{Mesh::Gltf, L"tmp/cube.gltf"};
    Mesh mesh2{Mesh::Gltf, L"tmp/cube.gltf"};
    Texture tex{Texture::Png, L"tmp/cube.png"};
    Material matl;
    matl.pbrmr().colorTex = &tex;

    Model mdl1{mesh1, matl};
    Model mdl2{mesh2};
    mdl2.setMaterial(&matl);
    mdl1.transform() = translate(2.0f, 2.0f, -2.0f);
    mdl2.transform() = rotateZ(3.14159265359f * 0.25f);

    Scene scn1;
    scn1.insert(mdl1);
    scn1.camera().place({0.0f, 30.0f, 30.0f});
    scn1.camera().point({});
    scn1.color() = {0.05f, 0.05f, 0.2f, 1.0f};
    Scene scn2;
    scn2.insert(mdl2);
    scn2.camera() = scn1.camera();
    scn2.color()[0] = 0.125f;

    WS_NS::onKbKey(onKey);

    auto scn = &scn1;

    view.loop(*scn, 60, [&](auto) {
      if (input.quit)
        return false;

      if (input.swapScn) {
        scn = scn == &scn1 ? &scn2 : &scn1;
        input.swapScn = false;
        view.swapScene(*scn);
        return true;
      }

      auto& cam = scn->camera();

      if (input.moveF)
        cam.moveForward(deltaM);
      if (input.moveB)
        cam.moveBackward(deltaM);
      if (input.moveL)
        cam.moveLeft(deltaM);
      if (input.moveR)
        cam.moveRight(deltaM);
      if (input.moveU)
        cam.moveUp(deltaM);
      if (input.moveD)
        cam.moveDown(deltaM);
      if (input.turnL)
        cam.turnLeft(deltaT);
      if (input.turnR)
        cam.turnRight(deltaT);
      if (input.turnU)
        cam.turnUp(deltaT);
      if (input.turnD)
        cam.turnDown(deltaT);
      if (input.zoomI)
        cam.zoomIn(deltaZ);
      if (input.zoomO)
        cam.zoomOut(deltaZ);
      if (input.place)
        cam.place({0.0f, 0.0f, 20.0f});
      if (input.point)
        cam.point({});

      return true;
    });

    return {{L"rendering", true}};
  }
};

RenderTest::Input RenderTest::input{};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* renderTest() {
  static RenderTest test;
  return &test;
}

TEST_NS_END
