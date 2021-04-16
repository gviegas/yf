//
// SG
// MaterialTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "UnitTests.h"
#include "Material.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct MaterialTest : Test {
  MaterialTest() : Test(L"Material") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    auto print = [](const Material& ml) {
      const auto& pbrmr = ml.pbrmr();
      const auto& normal = ml.normal();
      const auto& occlusion = ml.occlusion();
      const auto& emissive = ml.emissive();
      wcout << "\nMaterial:"
            << "\n Pbrmr:"
            << "\n  colorTex: " << pbrmr.colorTex
            << "\n  colorFac: [" << pbrmr.colorFac[0] << ", "
                                 << pbrmr.colorFac[1] << ", "
                                 << pbrmr.colorFac[2] << ", "
                                 << pbrmr.colorFac[3] << "]"
            << "\n  metalRoughTex: " << pbrmr.metalRoughTex
            << "\n  metalness: " << pbrmr.metalness
            << "\n  roughness: " << pbrmr.roughness
            << "\n Normal:"
            << "\n  texture: " << normal.texture
            << "\n  scale: " << normal.scale
            << "\n Occlusion:"
            << "\n  texture: " << occlusion.texture
            << "\n  strength: " << occlusion.strength
            << "\n Emissive:"
            << "\n  texture: " << emissive.texture
            << "\n  factor: [" << emissive.factor[0] << ", "
                               << emissive.factor[1] << ", "
                               << emissive.factor[2] << "]"
            << endl;
    };

    Material ml1{};
    print(ml1);

    Material::Occlusion occlusion{nullptr, 0.5f};
    Material::Emissive emissive{nullptr, {0.1f, 0.2f, 0.3f}};

    Material ml2{{}, {}, occlusion, emissive};
    print(ml2);

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::materialTest() {
  static MaterialTest test;
  return &test;
}
