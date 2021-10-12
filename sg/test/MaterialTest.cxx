//
// SG
// MaterialTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "Test.h"
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
            << "\n  colorTex: " << (pbrmr.colorTex ? "some" : "none")
            << "\n  colorFac: [" << pbrmr.colorFac[0] << ", "
                                 << pbrmr.colorFac[1] << ", "
                                 << pbrmr.colorFac[2] << ", "
                                 << pbrmr.colorFac[3] << "]"
            << "\n  metalRoughTex: " << (pbrmr.metalRoughTex ? "some" : "none")
            << "\n  metallic: " << pbrmr.metallic
            << "\n  roughness: " << pbrmr.roughness
            << "\n Normal:"
            << "\n  texture: " << (normal.texture ? "some" : "none")
            << "\n  scale: " << normal.scale
            << "\n Occlusion:"
            << "\n  texture: " << (occlusion.texture ? "some" : "none")
            << "\n  strength: " << occlusion.strength
            << "\n Emissive:"
            << "\n  texture: " << (emissive.texture ? " some" : "none")
            << "\n  factor: [" << emissive.factor[0] << ", "
                               << emissive.factor[1] << ", "
                               << emissive.factor[2] << "]"
            << endl;
    };

    auto isEqPbrmr = [](const Material::Pbrmr& a, const Material::Pbrmr& b) {
      if (a.colorTex != b.colorTex ||
          a.colorFac[0] != b.colorFac[0] || a.colorFac[1] != b.colorFac[1] ||
          a.colorFac[2] != b.colorFac[2] || a.colorFac[3] != b.colorFac[3] ||
          a.metalRoughTex != b.metalRoughTex ||
          a.metallic != b.metallic || a.roughness != b.roughness)
        return false;
      return true;
    };

    auto isEqNormal = [](const Material::Normal& a,
                         const Material::Normal& b) {
      if (a.texture != b.texture || a.scale != b.scale)
        return false;
      return true;
    };

    auto isEqOcclusion = [](const Material::Occlusion& a,
                            const Material::Occlusion& b) {
      if (a.texture != b.texture || a.strength != b.strength)
        return false;
      return true;
    };

    auto isEqEmissive = [](const Material::Emissive& a,
                           const Material::Emissive& b) {
      if (a.texture != b.texture || a.factor[0] != b.factor[0] ||
          a.factor[1] != b.factor[1] || a.factor[2] != b.factor[2])
        return false;
      return true;
    };

    Material ml1{};
    print(ml1);

    a.push_back({L"Material()", isEqPbrmr(ml1.pbrmr(), {}) &&
                                isEqNormal(ml1.normal(), {}) &&
                                isEqOcclusion(ml1.occlusion(), {}) &&
                                isEqEmissive(ml1.emissive(), {})});

    Material::Pbrmr mr{{}, {0.5f}, {}, 0.2f, 0.4f};

    Material ml2{};
    ml2.pbrmr() = {{}, {0.5f}, {}, 0.2f, 0.4f};
    print(ml2);

    a.push_back({L"Material()", isEqPbrmr(ml2.pbrmr(), mr) &&
                                isEqNormal(ml2.normal(), {}) &&
                                isEqOcclusion(ml2.occlusion(), {}) &&
                                isEqEmissive(ml2.emissive(), {})});

    Material::Occlusion occ{{}, 0.5f};
    Material::Emissive emsv{{}, {0.1f, 0.2f, 0.3f}};

    Material ml3{};
    ml3.occlusion() = {{}, 0.5f};
    ml3.emissive() = {{}, {0.1f, 0.2f, 0.3f}};
    print(ml3);

    a.push_back({L"Material()", isEqPbrmr(ml3.pbrmr(), {}) &&
                                isEqNormal(ml3.normal(), {}) &&
                                isEqOcclusion(ml3.occlusion(), occ) &&
                                isEqEmissive(ml3.emissive(), emsv)});

    Material ml4{ml3};
    a.push_back({L"Material(other)", isEqPbrmr(ml3.pbrmr(), ml4.pbrmr()) &&
                                     isEqNormal(ml3.normal(), ml4.normal()) &&
                                     isEqOcclusion(ml3.occlusion(),
                                                   ml4.occlusion()) &&
                                     isEqEmissive(ml3.emissive(),
                                                  ml4.emissive())});

    Material::Normal norm{{}, 0.01f};

    ml3.pbrmr() = {{}, {0.5f}, {}, 0.2f, 0.4f};
    ml3.normal() = {{}, 0.01f};
    print(ml3);
    a.push_back({L"modifiers", isEqPbrmr(ml3.pbrmr(), mr) &&
                               isEqNormal(ml3.normal(), norm) &&
                               isEqOcclusion(ml3.occlusion(), occ) &&
                               isEqEmissive(ml3.emissive(), emsv)});

    a.push_back({L"Material(other)", !isEqPbrmr(ml3.pbrmr(), ml4.pbrmr()) &&
                                     !isEqNormal(ml3.normal(), ml4.normal()) &&
                                     isEqOcclusion(ml3.occlusion(),
                                                   ml4.occlusion()) &&
                                     isEqEmissive(ml3.emissive(),
                                                  ml4.emissive())});
    ml4 = ml3;
    a.push_back({L"operator=", isEqPbrmr(ml3.pbrmr(), ml4.pbrmr()) &&
                               isEqNormal(ml3.normal(), ml4.normal()) &&
                               isEqOcclusion(ml3.occlusion(),
                                             ml4.occlusion()) &&
                               isEqEmissive(ml3.emissive(), ml4.emissive())});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* materialTest() {
  static MaterialTest test;
  return &test;
}

TEST_NS_END
