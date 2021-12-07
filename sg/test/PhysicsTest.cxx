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
    // TODO: Need to disallow copy
    PhysicsWorld physWorld = scene1.physicsWorld();
    Scene().physicsWorld() = physWorld;

    physWorld.enable();
    a.push_back({L"enabled()", true});

    physWorld.disable();
    a.push_back({L"disable()", true});

    return a;
  }
};

INTERNAL_NS_END
