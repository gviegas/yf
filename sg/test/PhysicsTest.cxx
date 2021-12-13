//
// SG
// PhysicsTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "InteractiveTest.h"
#include "PhysicsImpl.h"
#include "Scene.h"
#include "Node.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct PhysicsTest : InteractiveTest {
  PhysicsTest() : InteractiveTest(L"Physics", 640, 480) { }

  Assertions run(const vector<string>&) {
    Assertions a;

    Scene scene1;
    PhysicsWorld& physWorld = scene1.physicsWorld();
    bool check = physWorld.isEnabled();

    // Expected to be enabled by default
    a.push_back({L"PhysicsWorld()", check});

    physWorld.disable();
    a.push_back({L"disable()", !physWorld.isEnabled()});
    check = check && !physWorld.isEnabled();

    physWorld.enable();
    a.push_back({L"enable()", physWorld.isEnabled()});
    check = physWorld.isEnabled();

    physWorld.disable();
    check = check && !physWorld.isEnabled();
    physWorld.disable();
    check = check && !physWorld.isEnabled();
    physWorld.enable();
    check = check && physWorld.isEnabled();
    physWorld.enable();
    check = check && physWorld.isEnabled();
    a.push_back({L"isEnabled()", check});

    a.push_back(evalTest());

    return a;
  }

  // TODO: Check `PhysicsWorld::Impl` state for each evaluation
  Assertion evalTest() {
    Scene scene;

    Node node1;
    node1.name() = L"node1";
    node1.setBody(make_unique<Body>(BBox(2.0f)));
    node1.body()->setCategoryMask(16);

    Node node2;
    node2.name() = L"node2";
    node2.setBody(make_unique<Body>(BBox(2.0f)));
    node2.body()->setCategoryMask(1);

    Node node3;
    node3.name() = L"node3";
    node3.setBody(make_unique<Body>(BBox(2.0f)));
    node3.body()->setCategoryMask(1);

    Node node4;
    node4.name() = L"node4";
    node4.setBody(make_unique<Body>(BBox(2.0f)));
    node4.body()->setCategoryMask(0x80000002);

    wcout << "\n** PhysicsWorld state **\n\n";

    wcout << "scene.insert(node1)\n";
    wcout << "scene.insert(node2)\n";
    wcout << "scene.insert(node4)\n";
    scene.insert(node1);
    scene.insert(node2);
    scene.insert(node4);
    scene.physicsWorld().impl().evaluate();

    wcout << "\n----\n\n";
    wcout << "node1.drop()\n";
    wcout << "scene.insert(node3)\n";
    node1.drop();
    scene.insert(node3);
    scene.physicsWorld().impl().evaluate();

    wcout << "\n----\n\n";
    wcout << "node1.body()->setCategoryMask(0x80000001)\n";
    wcout << "scene.insert(node1)\n";
    node1.body()->setCategoryMask(0x80000001);
    scene.insert(node1);
    scene.physicsWorld().impl().evaluate();

    wcout << "\n----\n\n";
    wcout << "scene.prune()\n";
    scene.prune();
    scene.physicsWorld().impl().evaluate();

    wcout << "\n----\n\n";
    wcout << "node3.body()->setCategoryMask(0)\n";
    wcout << "scene.insert(node3)\n";
    wcout << "scene.insert(node2)\n";
    node3.body()->setCategoryMask(0);
    scene.insert(node3);
    scene.insert(node2);
    scene.physicsWorld().impl().evaluate();

    wcout << "\n----\n\n";
    wcout << "node2.drop()\n";
    wcout << "scene.insert(node2)\n";
    node2.drop();
    scene.insert(node2);
    scene.physicsWorld().impl().evaluate();

    wcout << "\n----\n\n";
    wcout << "node3.body()->setCategoryMask(0x40)\n";
    wcout << "node3.insert(node1)\n";
    node3.body()->setCategoryMask(0x40);
    node3.insert(node1);
    scene.physicsWorld().impl().evaluate();

    wcout << "\n----\n\n";
    wcout << "node1.body()->setCategoryMask(0x2000)\n";
    wcout << "node1.drop()\n";
    node1.body()->setCategoryMask(0x2000);
    node1.drop();
    scene.physicsWorld().impl().evaluate();

    wcout << "\n----\n\n";
    wcout << "node3.insert(node4)\n";
    wcout << "node4.body()->setCategoryMask(0x1)\n";
    node3.insert(node4);
    node4.body()->setCategoryMask(0x1);
    scene.physicsWorld().impl().evaluate();

    // TODO
    return {L"Impl::evaluate()", true};
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* physicsTest() {
  static PhysicsTest test;
  return &test;
}

TEST_NS_END
