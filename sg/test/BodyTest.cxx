//
// SG
// BodyTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "SG.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct BodyTest : Test {
  BodyTest() : Test(L"Body") { }

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

    Node node1, node2;
    body2.setNode(&node2);
    body2.setNode(nullptr);
    body1.setNode(&node1);
    body3.setNode(&node2);

    a.push_back({L"setNode()", body1.node() == &node1 && !body2.node() &&
                               body3.node() == &node2});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* bodyTest() {
  static BodyTest test;
  return &test;
}

TEST_NS_END
