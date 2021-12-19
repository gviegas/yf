//
// SG
// BodyTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "InteractiveTest.h"
#include "SG.h"
#include "yf/Except.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct BodyTest : InteractiveTest {
  BodyTest() : InteractiveTest(L"Body", 640, 480) { }

  Assertions run(const vector<string>&) {
    Assertions a;

    Sphere sphr1(1.0f);
    Sphere sphr2(3.5f, {1.0f, 0.0f, -1.0f});

    a.push_back({L"Sphere()", sphr1.radius == 1.0f && sphr1.t[0] == 0.0f &&
                              sphr1.t[1] == 0.0f && sphr1.t[2] == 0.0f &&
                              sphr2.radius == 3.5f && sphr2.t[0] == 1.0f &&
                              sphr2.t[1] == 0.0f && sphr2.t[2] == -1.0f});

    BBox bb1(Vec3f(2.0f));
    BBox bb2({0.25f, 3.0f, 2.5f}, Vec3f(5.0f));

    a.push_back({L"BBox()", bb1.extent[0] == 2.0f && bb1.extent[1] == 2.0f &&
                            bb1.extent[2] == 2.0f && bb1.t[0] == 0.0f &&
                            bb1.t[1] == 0.0f && bb1.t[2] == 0.0f &&
                            bb2.extent[0] == 0.25f && bb2.extent[1] == 3.0f &&
                            bb2.extent[2] == 2.5f && bb2.t[0] == 5.0f &&
                            bb2.t[1] == 5.0f && bb2.t[2] == 5.0f});

    Body body1(sphr1);
    Body body2(bb2);
    Body body3({&sphr1, &sphr2, &bb1, &bb2});

    // TODO: check shapes
    a.push_back({L"Body()", !body1.node() && !body2.node() && !body3.node()});

    interactive();

    return a;
  }

  void interactive() {
    Collection coll;
    coll.load("tmp/cube.glb");

    // Dup.
    Node* node1 = nullptr;
    Node* node2 = nullptr;
    Node* node = nullptr;
    for (auto& nd : coll.nodes()) {
      if (nd->name() == L"Cube") {
        Model* mdl = dynamic_cast<Model*>(nd.get());
        node = new Model();
        static_cast<Model*>(node)->setMesh(mdl->mesh());
        static_cast<Model*>(node)->setSkin(mdl->skin());
        nd->parent()->insert(*node);
        node->name() = L"Cube2";
        node->transform() = translate(0.0f, 15.0f, 0.0f);
        node1 = nd.get();
        node2 = node;
        break;
      }
    }
    if (node)
      coll.nodes().push_back(unique_ptr<Model>(static_cast<Model*>(node)));
    else
      throw yf::FileExcept("BodyTest interactive() - Invalid file");

    setObject(node);

    // Physics bodies
    Body body(BBox(2.0f));
    node1->setBody(body);
    node2->setBody(make_unique<Body>(Sphere(1.0f)));

    // Render
    auto scn = coll.scenes().front().get();

    bool isPlaying = false;

    update(*scn, [&](auto elapsedTime) {
      if (input.next || input.prev) {
        node = node == node1 ? node2 : node1;
        setObject(node);
        input.next = input.prev = false;
      }

      float dt = chrono::duration<float>(elapsedTime).count();

      auto& xform = node->transform();
      Mat3f rot{{xform[0][0], xform[0][1], xform[0][2]},
                {xform[1][0], xform[1][1], xform[1][2]},
                {xform[2][0], xform[2][1], xform[2][2]}};
      auto g = invert(rot) * Vec3f{0.0f, -9.8f * dt, 0.0f};
      xform *= translate(g);

      if (input.start) {
        if (!coll.animations().empty())
          isPlaying = true;
        input.start = false;
      } else if (input.stop) {
        if (!coll.animations().empty()) {
          isPlaying = false;
          coll.animations().back()->stop();
        }
        input.stop = false;
      }

      if (isPlaying)
        wcout << "\n completed ? "
              << (coll.animations().back()->play(elapsedTime) ? "no" : "yes");

      return true;
    });
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* bodyTest() {
  static BodyTest test;
  return &test;
}

TEST_NS_END
