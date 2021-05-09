//
// SG
// CollectionTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "SG.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct CollectionTest : Test {
  CollectionTest() : Test(L"Collection") { }

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

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* collectionTest() {
  static CollectionTest test;
  return &test;
}

TEST_NS_END
