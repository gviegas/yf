//
// SG
// CollectionTest.cxx
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

struct CollectionTest : Test {
  CollectionTest() : Test(L"Collection") { }

  struct Input {
    bool moveF, moveB, moveL, moveR, moveU, moveD;
    bool turnL, turnR, turnU, turnD;
    bool zoomI, zoomO;
    bool place, point;
    bool mode;
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
    CollectionTest::key = key;

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
    case WS_NS::KeyCodeEsc:
      input.quit = b;
      break;
    default:
      break;
    }
  }

  Assertions run(const vector<string>&) {
    Assertions a;

    Collection coll;

    a.push_back({L"Collection()", coll.scenes().empty() &&
                                  coll.nodes().empty() &&
                                  coll.meshes().empty() &&
                                  coll.textures().empty() &&
                                  coll.materials().empty() &&
                                  coll.skins().empty()});

    coll.scenes().push_back(make_unique<Scene>());
    coll.scenes().back()->name() = L"scn1";
    Scene scn;
    scn.name() = L"scn2";
    coll.scenes().push_back(make_unique<Scene>(scn));

    a.push_back({L"scenes()", coll.scenes().size() == 2 &&
                              coll.scenes().front()->name() == L"scn1" &&
                              coll.scenes().back()->name() == L"scn2"});

    auto count = 100;
    Node nd;
    nd.name() = L"node";
    while (count--)
      coll.nodes().push_back(make_unique<Node>(nd));

    a.push_back({L"nodes()", coll.nodes().size() == 100 &&
                             coll.nodes().front()->name() == nd.name() &&
                             coll.nodes().back()->name() == nd.name()});

    Mesh mesh{Mesh::Gltf, L"tmp/cube.gltf"};
    coll.meshes().push_back(mesh);
    coll.meshes().push_back({Mesh::Gltf, L"tmp/cube.gltf"});
    coll.meshes().push_back(coll.meshes().front());

    a.push_back({L"meshes()", coll.meshes().size() == 3 &&
                              &coll.meshes()[0].impl() == &mesh.impl() &&
                              &coll.meshes()[1].impl() != &mesh.impl() &&
                              &coll.meshes()[2].impl() == &mesh.impl()});

    coll.textures().push_back({Texture::Png, L"tmp/cube.png"});
    Texture tex{Texture::Png, L"tmp/cube.png"};
    coll.textures().push_back(tex);
    coll.textures().push_back(tex);

    a.push_back({L"textures()", coll.textures().size() == 3 &&
                                &coll.textures()[0].impl() !=
                                  &coll.textures()[1].impl() &&
                                &coll.textures()[1].impl() ==
                                  &coll.textures()[2].impl() &&
                                &coll.textures()[2].impl() == &tex.impl()});

    coll.materials().push_back({});
    coll.materials().front().normal() = {coll.textures()[0], 0.25f};
    coll.materials().push_back({{}, {}, {}, {}});
    Material matl{{}, {}, {tex, 0.5f}, {}};
    coll.materials().push_back(matl);
    coll.materials().push_back(matl);

    a.push_back({L"materials()",
                 coll.materials().size() == 4 &&
                 coll.materials()[0].normal().texture == coll.textures()[0] &&
                 coll.materials()[0].normal().scale == 0.25f &&
                 coll.materials()[2].occlusion().texture == tex &&
                 coll.materials()[3].occlusion().strength == 0.5f &&
                 !coll.materials()[3].normal().texture});

    coll.skins().push_back({1, {}});
    Skin skin(2, {});
    coll.skins().push_back(skin);

    a.push_back({L"skins()", coll.skins().size() == 2 &&
                             coll.skins().front().joints().size() == 1 &&
                             coll.skins().front().inverseBind().empty() &&
                             coll.skins().back().joints().size() == 2 &&
                             coll.skins().back().inverseBind().empty()});

    vector<Animation::Timeline> inputs({{1.0f}});
    vector<Animation::Scale> outS({{Vec3f{2.0f, 2.0f, 2.0f}}});
    coll.animations().push_back({inputs, {}, {}, outS});
    Node nd1, nd2;
    Animation::Action act1{&nd1, Animation::S, Animation::Step, 0, 0};
    Animation::Action act2{&nd2, Animation::S, Animation::Step, 0, 0};
    coll.animations().front().actions().push_back(act1);
    coll.animations().front().actions().push_back(act2);

    a.push_back({L"animations()",
                 coll.animations().size() == 1 &&
                 coll.animations().front().actions().size() == 2 &&
                 coll.animations().front().inputs().size() == 1 &&
                 coll.animations().front().outT().empty() &&
                 coll.animations().front().outR().empty() &&
                 coll.animations().front().outS().size() == 1});

    fromFile();
    return a;
  }

  void fromFile() {
    Collection coll(L"tmp/fullscene.gltf");
    //Collection coll(L"tmp/fullscene2.gltf");
    //Collection coll(L"tmp/fullscene3.gltf");

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
    auto win = WS_NS::createWindow(480, 400, L"Collection Test");
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

    view.loop(*scn, 60, [&](auto) {
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

      return true;
    });
  }
};

CollectionTest::Input CollectionTest::input{};
WS_NS::KeyCode CollectionTest::key = WS_NS::KeyCodeUnknown;

INTERNAL_NS_END

TEST_NS_BEGIN

Test* collectionTest() {
  static CollectionTest test;
  return &test;
}

TEST_NS_END
