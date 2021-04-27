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
#include "Material.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct ModelTest : Test {
  ModelTest() : Test(L"Model") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    Model mdl1;

    a.push_back({L"Model()", !mdl1.mesh() && !mdl1.material()});

    Mesh mesh{Mesh::FileType::Gltf, L"tmp/cube.gltf"};
    Model mdl2{mesh};

    a.push_back({L"Model(mesh)", mdl2.mesh() == &mesh && !mdl2.material()});

    Material material;
    Model mdl3{mesh, material};

    a.push_back({L"Model(mesh, material)", mdl3.mesh() == &mesh &&
                                           mdl3.material() == &material});

    Model mdl4{mdl3};
    a.push_back({L"Model(other)", mdl4.mesh() == &mesh &&
                                  mdl4.material() == &material &&
                                  mdl4.mesh() == mdl3.mesh() &&
                                  mdl4.material() == mdl3.material()});

    Model mdl5 = mdl3;
    a.push_back({L"=", mdl5.mesh() == &mesh && mdl5.material() == &material &&
                       mdl5.mesh() == mdl3.mesh() &&
                       mdl5.material() == mdl3.material()});

    mdl1.setMesh(&mesh);
    mdl1.setMaterial(&material);
    mdl2.setMaterial(nullptr);
    mdl3.setMesh(nullptr);

    a.push_back({L"set*()", mdl1.mesh() == &mesh &&
                            mdl1.material() == &material &&
                            mdl2.mesh() == &mesh  && !mdl2.material() &&
                            !mdl3.mesh() && mdl3.material() == &material});

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
