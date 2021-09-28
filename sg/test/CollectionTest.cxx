//
// SG
// CollectionTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "yf/ws/WS.h"

#include "InteractiveTest.h"
#include "SG.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct CollectionTest : InteractiveTest {
  CollectionTest() : InteractiveTest(L"Collection", 640, 480) { }

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

    auto mesh = new Mesh(L"tmp/cube.glb");
    coll.meshes().push_back(unique_ptr<Mesh>(mesh));
    coll.meshes().push_back(make_unique<Mesh>(L"tmp/cube.glb"));

    a.push_back({L"meshes()", coll.meshes().size() == 2 &&
                              coll.meshes()[0].get() == mesh &&
                              &coll.meshes()[0]->impl() == &mesh->impl()});

    coll.skins().push_back(make_unique<Skin>(1, vector<Mat4f>()));
    auto skin = new Skin(2, {});
    coll.skins().push_back(unique_ptr<Skin>(skin));

    a.push_back({L"skins()", coll.skins().size() == 2 &&
                             coll.skins().front()->joints().size() == 1 &&
                             coll.skins().front()->inverseBind().empty() &&
                             coll.skins().back()->joints().size() == 2 &&
                             coll.skins().back()->inverseBind().empty()});

    coll.textures().push_back(make_unique<Texture>(L"tmp/cube.png"));
    auto tex = new Texture(L"tmp/cube.png");
    coll.textures().push_back(unique_ptr<Texture>(tex));

    a.push_back({L"textures()",
                 coll.textures().size() == 2 &&
                 &coll.textures()[0]->impl() != &coll.textures()[1]->impl()});

    coll.materials().push_back(make_unique<Material>());
    coll.materials().front()->normal() = {coll.textures()[0].get(), 0.25f};
    coll.materials().push_back(make_unique<Material>());
    auto matl = new Material({}, {}, {tex, 0.5f}, {});
    coll.materials().push_back(Material::Ptr(matl));

    a.push_back({L"materials()",
                 coll.materials().size() == 3 &&
                 coll.materials()[0]->normal().texture ==
                  coll.textures()[0].get() &&
                 coll.materials()[0]->normal().scale == 0.25f &&
                 coll.materials()[2]->occlusion().texture == tex &&
                 coll.materials()[2]->occlusion().strength == 0.5f &&
                 !coll.materials()[2]->normal().texture});

    vector<Animation::Timeline> inputs({{1.0f}});
    vector<Animation::Translation> outT;
    vector<Animation::Rotation> outR({{Qnionf(1.0f, {})}});
    vector<Animation::Scale> outS({{Vec3f(2.0f)}});
    coll.animations().push_back(make_unique<Animation>(inputs,
                                                       outT, outR, outS));
    Node nd1, nd2;
    Animation::Action act1{&nd1, Animation::S, Animation::Step, 0, 0};
    Animation::Action act2{&nd2, Animation::S, Animation::Step, 0, 0};
    coll.animations().front()->actions().push_back(act1);
    coll.animations().front()->actions().push_back(act2);

    a.push_back({L"animations()",
                 coll.animations().size() == 1 &&
                 coll.animations().front()->actions().size() == 2 &&
                 coll.animations().front()->inputs().size() == 1 &&
                 coll.animations().front()->outT().empty() &&
                 coll.animations().front()->outR().size() == 1 &&
                 coll.animations().front()->outS().size() == 1});

    fromFile();
    return a;
  }

  void fromFile() {
    Collection coll(L"tmp/scene.glb");

    auto printMatrix = [](const Mat4f& mat) {
      for (size_t i = 0; i < mat.rows(); i++) {
        wcout << "\n\t";
        for (size_t j = 0; j < mat.columns(); j++)
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

    wcout << "\n Skins: #" << coll.skins().size();
    for (const auto& sk : coll.skins()) {
      wcout << "\n  Skin:"
            << "\n   joints: #" <<  sk->joints().size();
      for (const auto& jt : sk->joints()) {
        wcout << "\n\n   `" << jt->name() << "`";
        printMatrix(jt->transform());
      }
      wcout << "\n\n   inverseBind: #" << sk->inverseBind().size();
      wcout << "\n";
      for (const auto& ib : sk->inverseBind())
        printMatrix(ib);
    }

    wcout << "\n Textures: #" << coll.textures().size();

    wcout << "\n Materials: #" << coll.materials().size();
    for (const auto& matl : coll.materials()) {
      wcout << "\n  Material:"
            << "\n   pbrmr:"
            << "\n    colorTex: " << (matl->pbrmr().colorTex ? 'y':'n')
            << "\n    colorFac: [" << matl->pbrmr().colorFac[0] << ", "
                                   << matl->pbrmr().colorFac[1] << ", "
                                   << matl->pbrmr().colorFac[2] << ", "
                                   << matl->pbrmr().colorFac[3] << "]"
            << "\n    metalRoughTex: " << (matl->pbrmr().metalRoughTex ?
                                           'y':'n')
            << "\n    metallic: " << matl->pbrmr().metallic
            << "\n    roughness: " << matl->pbrmr().roughness
            << "\n   normal:"
            << "\n    texture: " << (matl->normal().texture ? 'y':'n')
            << "\n    scale: " << matl->normal().scale
            << "\n   occlusion:"
            << "\n    texture: " << (matl->occlusion().texture ? 'y':'n')
            << "\n    strength: " << matl->occlusion().strength
            << "\n   emissive:"
            << "\n    texture: " << (matl->emissive().texture ? 'y' : 'n')
            << "\n    factor: [" << matl->emissive().factor[0] << ", "
                                 << matl->emissive().factor[1] << ", "
                                 << matl->emissive().factor[2] << "]";
    }

    wcout << "\n Animations: #" << coll.animations().size();
    for (const auto& an: coll.animations()) {
      wcout << "\n  Animation `" << an->name() << "`:"
            << "\n   actions: #" << an->actions().size();
      for (const auto& act: an->actions())
        wcout << "\n    `" << act.target->name() << "`|"
                           << act.type << "|" << act.method << "|"
                           << act.input << "|" << act.output;
      wcout << "\n   inputs: #" << an->inputs().size();
      for (const auto& in : an->inputs()) {
        wcout << "\n    *";
        for (const auto& k : in)
          wcout << "\n     " << k;
      }
      wcout << "\n   outT: #" << an->outT().size();
      for (const auto& t : an->outT()) {
        wcout << "\n    *";
        for (const auto& v : t)
          wcout << "\n     [" << v[0] << ", " << v[1] << ", " << v[2] << "]";
      }
      wcout << "\n   outR: #" << an->outR().size();
      for (const auto& r : an->outR()) {
        wcout << "\n    *";
        for (const auto& q : r)
          wcout << "\n     (" << q.r() << ", [" << q.v()[0] << ", "
                              << q.v()[1] << ", " << q.v()[2] << "])";
      }
      wcout << "\n   outS: #" << an->outS().size();
      for (const auto& s : an->outS()) {
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

    setObject(obj);
    update(*scn, {});
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* collectionTest() {
  static CollectionTest test;
  return &test;
}

TEST_NS_END
