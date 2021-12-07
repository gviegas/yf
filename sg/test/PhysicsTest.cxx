//
// SG
// PhysicsTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "InteractiveTest.h"
#include "Physics.h"
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
    bool check;

    physWorld.enable();
    check = physWorld.isEnabled();
    a.push_back({L"enable()", check});

    physWorld.disable();
    check = check && !physWorld.isEnabled();
    a.push_back({L"disable()", !physWorld.isEnabled()});

    physWorld.enable();
    check = check && physWorld.isEnabled();
    physWorld.disable();
    check = check && !physWorld.isEnabled();
    a.push_back({L"isEnabled()", check});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* physicsTest() {
  static PhysicsTest test;
  return &test;
}

TEST_NS_END
