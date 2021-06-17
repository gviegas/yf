//
// SG
// AnimationTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "yf/ws/WS.h"

#include "Test.h"
#include "SG.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct AnimationTest : Test {
  AnimationTest() : Test(L"Animation") { }

  struct Input {
    bool moveF, moveB, moveL, moveR, moveU, moveD;
    bool turnL, turnR, turnU, turnD;
    bool zoomI, zoomO;
    bool place, point;
    bool mode;
    bool play, stop;
    bool quit;
  };

  static Input input;
  static WS_NS::KeyCode key;
  static constexpr float deltaM = 0.215f;
  static constexpr float deltaT = 0.025f;
  static constexpr float deltaZ = 0.035f;

  static void onKey(WS_NS::KeyCode key, WS_NS::KeyState state,
                    [[maybe_unused]] WS_NS::KeyModMask modMask) {

    const bool b = state == WS_NS::KeyStatePressed;
    AnimationTest::key = key;

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
    case WS_NS::KeyCodeM:
      input.mode = b;
      break;
    case WS_NS::KeyCodeZ:
      input.play = b;
      break;
    case WS_NS::KeyCodeX:
      input.stop = b;
      break;
    case WS_NS::KeyCodeEsc:
      input.quit = b;
      break;
    default:
      break;
    }
  }

  Assertions run(const vector<string>&) {
    Assertions a;

    vector<Animation::Timeline> inputs{{0.5f, 1.0f}, {0.33f, 0.67f, 1.0f}};
    vector<Animation::Translation> outT{
      {Vec3f{1.0f, 1.0f, 1.0f}, Vec3f{0.0f, 0.0f, 2.0f}},
      {Vec3f{2.0f, 4.0f, 0.0f}, Vec3f{5.0f}, Vec3f{-3.0f}}};

    Animation an1(inputs, outT, {}, {});

    a.push_back({L"Animation(...)", an1.actions().empty() &&
                                    an1.inputs().size() == 2 &&
                                    an1.outT().size() == 2 &&
                                    an1.outR().empty() && an1.outS().empty()});

    Animation an2;

    a.push_back({L"Animation()", !an2});

    Animation an3(an1);
    Animation an4 = an3;

    a.push_back({L"Animation(other), =", an3.actions().empty() &&
                                         an4.actions().empty() &&
                                         an3.inputs().size() == 2 &&
                                         an4.inputs().size() == 2 &&
                                         an3.outT().size() == 2 &&
                                         an4.outT().size() == 2 &&
                                         an3.outR().empty() &&
                                         an4.outR().empty() &&
                                         an3.outS().empty() &&
                                         an4.outS().empty()});

    Node nd;
    Animation::Action act{&nd, Animation::T, Animation::Step, 1, 1};
    an1.actions().push_back(act);

    a.push_back({L"actions()", an1.actions().size() == 1 &&
                               an3.actions().size() == 1 &&
                               an1.actions().front().input == 1 &&
                               an4.actions().front().output == 1});

    a.push_back({L"bool, !", an1 && !an2 && an3 && an4 && !Animation(an2)});

    a.push_back({L"==, !=", an1 == an3 && an4 == an1 && an1 != an2 &&
                            an2 == Animation()});

    an1.name() = L"an1";

    a.push_back({L"name()", an1.name() == L"an1" && an1.name() == an4.name()});

    fromFile();
    return a;
  }

  void fromFile() {
    //Collection coll(L"tmp/animation.gltf");
    //Collection coll(L"tmp/animation2.gltf");
    Collection coll(L"tmp/animation3.gltf");

    auto printMatrix = [](const Mat4f& mat) {
      for (size_t i = 0; i < mat.rows(); ++i) {
        wcout << "\n\t";
        for (size_t j = 0; j < mat.columns(); ++j)
          wcout << mat[j][i] << "\t";
      }
      wcout << endl;
    };

    // Print
    wcout << "\nCollection:";
    wcout << "\n Scenes: #" << coll.scenes().size();
    for (const auto& scn: coll.scenes()) {
      wcout << "\n  Scene `" << scn->name() << "`:";
      wcout << "\n   children: ";
      for (const auto& chd : scn->children())
        wcout << "\n    `" << chd->name() << "`";
    }

    wcout << "\n Nodes: #" << coll.nodes().size();
    for (const auto& nd: coll.nodes()) {
      wcout << "\n  Node `" << nd->name() << "`:";
      if (nd->parent())
        wcout << "\n   parent: `" << nd->parent()->name() << "`";
      else
        wcout << "\n   (no parent)";
      if (nd->isLeaf()) {
        wcout << "\n   (no children)";
      } else {
        wcout << "\n   children: ";
        for (const auto& chd : nd->children())
          wcout << "\n    `" << chd->name() << "`";
      }
    }

    wcout << "\n Meshes: #" << coll.meshes().size();
    wcout << "\n Textures: #" << coll.textures().size();

    wcout << "\n Materials: #" << coll.materials().size();
    for (const auto& matl : coll.materials()) {
      wcout << "\n  Material:"
            << "\n   pbrmr:"
            << "\n    colorTex: " << (matl.pbrmr().colorTex ? 'y':'n')
            << "\n    colorFac: [" << matl.pbrmr().colorFac[0] << ", "
                                   << matl.pbrmr().colorFac[1] << ", "
                                   << matl.pbrmr().colorFac[2] << ", "
                                   << matl.pbrmr().colorFac[3] << "]"
            << "\n    metalRoughTex: " << (matl.pbrmr().metalRoughTex ? 'y':'n')
            << "\n    metallic: " << matl.pbrmr().metallic
            << "\n    roughness: " << matl.pbrmr().roughness
            << "\n   normal:"
            << "\n    texture: " << (matl.normal().texture ? 'y':'n')
            << "\n    scale: " << matl.normal().scale
            << "\n   occlusion:"
            << "\n    texture: " << (matl.occlusion().texture ? 'y':'n')
            << "\n    strength: " << matl.occlusion().strength
            << "\n   emissive:"
            << "\n    texture: " << (matl.emissive().texture ? 'y' : 'n')
            << "\n    factor: [" << matl.emissive().factor[0] << ", "
                                 << matl.emissive().factor[1] << ", "
                                 << matl.emissive().factor[2] << "]";
    }

    wcout << "\n Skins: #" << coll.skins().size();
    for (const auto& sk : coll.skins()) {
      wcout << "\n  Skin:"
            << "\n   joints: #" <<  sk.joints().size();
      for (const auto& jt : sk.joints()) {
        wcout << "\n\n   `" << jt->name() << "`";
        printMatrix(jt->transform());
      }
      wcout << "\n\n   inverseBind: #" << sk.inverseBind().size();
      wcout << "\n";
      for (const auto& ib : sk.inverseBind())
        printMatrix(ib);
    }

    wcout << "\n Animations: #" << coll.animations().size();
    for (const auto& an: coll.animations()) {
      wcout << "\n  Animation `" << an.name() << "`:"
            << "\n   actions: #" << an.actions().size();
      for (const auto& act: an.actions())
        wcout << "\n    `" << act.target->name() << "`|"
                           << act.type << "|" << act.method << "|"
                           << act.input << "|" << act.output;
      wcout << "\n   inputs: #" << an.inputs().size();
      for (const auto& in : an.inputs()) {
        wcout << "\n    *";
        for (const auto& k : in)
          wcout << "\n     " << k;
      }
      wcout << "\n   outT: #" << an.outT().size();
      for (const auto& t : an.outT()) {
        wcout << "\n    *";
        for (const auto& v : t)
          wcout << "\n     [" << v[0] << ", " << v[1] << ", " << v[2] << "]";
      }
      wcout << "\n   outR: #" << an.outR().size();
      for (const auto& r : an.outR()) {
        wcout << "\n    *";
        for (const auto& q : r)
          wcout << "\n     (" << q.r() << ", [" << q.v()[0] << ", "
                              << q.v()[1] << ", " << q.v()[2] << "])";
      }
      wcout << "\n   outS: #" << an.outS().size();
      for (const auto& s : an.outS()) {
        wcout << "\n    *";
        for (const auto& v : s)
          wcout << "\n     [" << v[0] << ", " << v[1] << ", " << v[2] << "]";
      }
    }

    wcout << endl;

#if 0
    return;
#endif

    // Render
    auto win = WS_NS::createWindow(480, 400, name_);
    View view(win.get());

    WS_NS::onKbKey(onKey);

    auto scn = coll.scenes().front().get();
    scn->camera().place({10.0f, 10.0f, 10.0f});
    scn->camera().point({});

    Node* obj = nullptr;
    for (auto& nd : coll.nodes()) {
      if (nd->name() == L"Cube") {
        obj = nd.get();
        break;
      }
    }

    bool camMode = true;
    bool isPlaying = false;

    view.loop(*scn, 60, [&](auto elapsedTime) {
      if (input.quit)
        return false;

      auto& cam = scn->camera();

      if (input.mode) {
        camMode = !camMode;
        input.mode = false;
      }

      if (camMode || !obj) {
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
      } else {
        Vec3f t;
        Qnionf r(1.0f, {});
        if (input.moveF)
          t[2] += 0.1f;
        if (input.moveB)
          t[2] -= 0.1f;
        if (input.moveL)
          t[0] += 0.1f;
        if (input.moveR)
          t[0] -= 0.1f;
        if (input.moveU)
          t[1] += 0.1f;
        if (input.moveD)
          t[1] -= 0.1f;
        if (input.turnL)
          r *= rotateQY(0.1f);
        if (input.turnR)
          r *= rotateQY(-0.1f);
        if (input.turnU)
          r *= rotateQX(-0.1f);
        if (input.turnD)
          r *= rotateQX(0.1f);
        if (input.place)
          t = {0.0f, 0.0f, 0.0f};
        obj->transform() *= translate(t) * rotate(r);
      }

      if (input.play) {
        if (!coll.animations().empty())
          isPlaying = true;
        input.play = false;
      } else if (input.stop) {
        if (!coll.animations().empty()) {
          isPlaying = false;
          coll.animations().back().stop();
        }
        input.stop = false;
      }

      if (isPlaying)
        wcout << "\n completed ? "
              << (coll.animations().back().play(elapsedTime) ? "no" : "yes");

      return true;
    });
  }
};

AnimationTest::Input AnimationTest::input{};
WS_NS::KeyCode AnimationTest::key = WS_NS::KeyCodeUnknown;

INTERNAL_NS_END

TEST_NS_BEGIN

Test* animationTest() {
  static AnimationTest test;
  return &test;
}

TEST_NS_END
