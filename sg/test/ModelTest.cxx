//
// SG
// ModelTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "Test.h"
#include "Model.h"
#include "Mesh.h"
#include "Skin.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ModelTest : Test {
  ModelTest() : Test(L"Model") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    Model mdl1;

    Mesh mesh{"test/data/cube.glb"};
    Model mdl2;
    mdl2.setMesh(&mesh);

    a.push_back({L"Model()", !mdl1.mesh() && !mdl1.skin() &&
                             mdl2.mesh() == &mesh && !mdl2.skin()});

    Skin skin(1, {});
    Model mdl3{mesh, skin};

    a.push_back({L"Model(mesh, skinl)", mdl3.mesh() == &mesh &&
                                        mdl3.skin() == &skin});

    Model mdl4{mdl3};
    a.push_back({L"Model(other)", mdl4.mesh() == &mesh &&
                                  mdl4.skin() == &skin &&
                                  mdl4.mesh() == mdl3.mesh() &&
                                  mdl4.skin() == mdl3.skin()});

    Model mdl5 = mdl3;
    a.push_back({L"=", mdl5.mesh() == &mesh &&
                       mdl5.skin() == &skin &&
                       mdl5.mesh() == mdl3.mesh() &&
                       mdl5.skin() == mdl3.skin()});

    mdl1.setMesh(&mesh);
    mdl1.setSkin(&skin);

    mdl3.setMesh(nullptr);
    mdl4.setSkin(nullptr);

    a.push_back({L"set*()", mdl1.mesh() == &mesh &&
                            mdl1.skin() == &skin &&
                            mdl2.mesh() == &mesh &&
                            !mdl2.skin() &&
                            !mdl3.mesh() &&
                            mdl3.skin() == &skin &&
                            mdl4.mesh() == &mesh &&
                            !mdl4.skin()});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* modelTest() {
  static ModelTest test;
  return &test;
}

TEST_NS_END
