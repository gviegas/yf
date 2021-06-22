//
// SG
// RenderTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "yf/ws/WS.h"

#include "InteractiveTest.h"
#include "SG.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct RenderTest : InteractiveTest {
  RenderTest() : InteractiveTest(L"Render", 640, 480) { }

  Assertions run(const vector<string>&) {
    Assertions a;

    // Resources
    Mesh mesh1{Mesh::Gltf, L"tmp/cube.gltf"};
    Mesh mesh2{Mesh::Gltf, L"tmp/cube.gltf"};
    Texture tex1{Texture::Png, L"tmp/cube.png"};
    Texture tex2{Texture::Png, L"tmp/cube2.png"};
    Material matl1;
    matl1.pbrmr().colorTex = tex1;
    matl1.pbrmr().metallic = 1.0f;
    matl1.pbrmr().roughness = 0.2f;
    Material matl2;
    matl2.pbrmr().colorTex = tex2;
    matl2.pbrmr().colorFac[3] = 0.25f;
    matl2.pbrmr().metallic = 0.5f;
    matl2.pbrmr().roughness = 1.0f;
    Skin skin1{};

    // Scene #1 contents
    const size_t instMdlN = 6;
    vector<Model> mdls{instMdlN, {mesh1, matl1, skin1}};
    mdls.push_back({mesh1, matl2, skin1});
    mdls.push_back({mesh2, matl1, skin1});
    mdls.push_back({mesh1, matl1, skin1});
    mdls.push_back({mesh2, matl2, skin1});

    auto tf = -static_cast<float>(mdls.size());
    for (auto& mdl : mdls) {
      mdl.transform() = scale(0.5f, 0.5f, 0.5f) * translate(tf, tf, -tf);
      tf += 2.0f;
    }

    Scene scn1;
    for (auto& mdl : mdls)
      scn1.insert(mdl);
    scn1.camera().place({0.0f, 0.0f, 20.0f});
    scn1.camera().point({});
    scn1.color() = {0.05f, 0.05f, 0.2f, 1.0f};

    // Scene #2 contents
    Model mdl1{mesh1, matl1, skin1};
    Model mdl2{mesh2, matl2, skin1};
    Model mdl3{mdl1};
    Model mdl4{mdl2};
    mdl1.transform() = translate(-3.0f, 0.0f, 0.0f);
    mdl2.transform() = translate(3.0f, 0.0f, 0.0f);
    mdl3.transform() = translate(0.0f, -3.0f, 0.0f);
    mdl4.transform() = translate(0.0f, 3.0f, 0.0f);

    Node grp1{};
    Node grp2{};
    Node grp3{};
    grp1.insert({&mdl1, &mdl2});
    grp2.insert({&mdl3, &grp3});
    grp3.insert(mdl4);
    grp2.transform() = scale(0.5f, 0.5f, 0.5f);
    grp3.transform() = scale(0.5f, 0.5f, 0.5f);

    Scene scn2;
    scn2.insert(grp1);
    scn2.insert(grp2);
    scn2.transform() = rotateZ(3.14159265358979f / 4.0f);
    scn2.camera() = scn1.camera();
    scn2.color()[0] = 0.125f;

    auto scn = &scn1;

    // Render
    update(*scn, [&](auto) {
      if (input.primary) {
        scn = scn == &scn1 ? &scn2 : &scn1;
        input.primary = false;
        setScene(scn);
      }
      return true;
    });

    return {{L"rendering", true}};
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* renderTest() {
  static RenderTest test;
  return &test;
}

TEST_NS_END
