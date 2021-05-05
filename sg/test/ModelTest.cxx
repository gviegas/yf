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

    Mesh mesh{Mesh::FileType::Gltf, L"tmp/cube.gltf"};
    Model mdl2;
    mdl2.setMesh(mesh);

    a.push_back({L"Model()", !mdl1.mesh() && !mdl1.skin() &&
                             mdl2.mesh() == mesh && !mdl2.skin()});

    Material material;
    Skin skin({Mat4f::identity()}, {});
    Model mdl3{mesh, material, skin};

    a.push_back({L"Model(mesh, material, skin)",
                 mdl3.mesh() == mesh && mdl3.material() == material &&
                 mdl3.skin() == skin});

    Model mdl4{mdl3};
    a.push_back({L"Model(other)", mdl4.mesh() == mesh &&
                                  mdl4.material() == material &&
                                  mdl4.skin() == skin &&
                                  mdl4.mesh() == mdl3.mesh() &&
                                  mdl4.material() == mdl3.material() &&
                                  mdl4.skin() == mdl3.skin()});

    Model mdl5 = mdl3;
    a.push_back({L"=", mdl5.mesh() == mesh && mdl5.material() == material &&
                       mdl5.skin() == skin && mdl5.mesh() == mdl3.mesh() &&
                       mdl5.material() == mdl3.material() &&
                       mdl5.skin() == mdl3.skin()});

    mdl1.setMesh(mesh);
    mdl1.setMaterial(material);
    mdl1.setSkin(skin);

    Material matl0;
    Mesh mesh0;
    Skin skin0;
    mdl2.setMaterial(matl0);
    mdl3.setMesh(mesh0);
    mdl4.setSkin(skin0);

    a.push_back({L"set*()", mdl1.mesh() == mesh &&
                            mdl1.material() == material &&
                            mdl1.skin() == skin && mdl2.mesh() == mesh &&
                            !mdl2.skin() && !mdl3.mesh() &&
                            mdl3.material() == material &&
                            mdl3.skin() == skin && mdl4.mesh() == mesh &&
                            mdl4.material() == material && !mdl4.skin()});

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
