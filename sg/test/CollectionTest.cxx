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
                                  coll.models().empty() &&
                                  coll.nodes().empty() &&
                                  coll.meshes().empty() &&
                                  coll.textures().empty() &&
                                  coll.materials().empty() &&
                                  coll.skins().empty()});

    coll.scenes().push_back({});
    coll.scenes().back().name() = L"scn1";
    Scene scn;
    scn.name() = L"scn2";
    coll.scenes().push_back(scn);

    a.push_back({L"scenes()", coll.scenes().size() == 2 &&
                              coll.scenes().front().name() == L"scn1" &&
                              coll.scenes().back().name() == L"scn2"});

    coll.models().push_back({});
    coll.models().push_back({});
    coll.models()[0].name() = L"mdl1";
    coll.models()[1].name() = L"mdl2";
    Model mdl;
    mdl.name() = L"mdl3";
    coll.models().push_back(mdl);
    coll.models().push_back({});

    a.push_back({L"models()", coll.models().size() == 4 &&
                              coll.models()[0].name() == L"mdl1" &&
                              coll.models()[1].name() == L"mdl2" &&
                              coll.models()[2].name() == L"mdl3" &&
                              coll.models()[3].name() == L""});

    auto count = 100;
    Node nd;
    nd.name() = L"node";
    while (count--)
      coll.nodes().push_back(nd);

    a.push_back({L"nodes()", coll.nodes().size() == 100 &&
                             coll.nodes().front().name() == nd.name() &&
                             coll.nodes().back().name() == nd.name()});

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

    coll.skins().push_back({{Mat4f()}, {}});
    Skin skin({Mat4f::identity(), Mat4f()}, {});
    coll.skins().push_back(skin);

    a.push_back({L"skins()", coll.skins().size() == 2 &&
                             coll.skins().front().joints().size() == 1 &&
                             coll.skins().back().joints().size() == 2 &&
                             &coll.skins().back().joints()[1].skin_ != &skin &&
                             coll.skins().back().joints()[1].index_ ==
                               skin.joints()[1].index_});

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
