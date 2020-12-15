//
// SG
// VectorTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <iostream>

#include "UnitTests.h"
#include "Vector.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct VectorTest : Test {
  VectorTest() : Test(L"Vector") { }

#define SG_PRINTSCAL(scal) wcout << #scal ": " << scal << endl
#define SG_PRINTVEC(vec) do { \
  wcout << #vec ": "; \
  for (const auto& x : vec) \
    wcout << x << " "; \
  wcout << endl; \
} while (false)

  Assertions run(const vector<string>&) {
    Assertions a;

    Vec2i v1{-1, 44};
    Vec3u v2{1, 0, 0xff};
    Vec4f v3{1.0f, -1.0f, 5023.01f};
    Vec3d v4{0.04502119805};
    v4.operator[](2) = -1.0/2.0;
    --v3[1];

    a.push_back({L"Vector(...)", v1[0] == -1 && v1[1] == 44 &&
                                 v2[0] == 1 && v2[1] == 0 && v2[2] == 0xff &&
                                 v3[0] == 1.0f && v3[1] == -2.0f &&
                                 v3[2] == 5023.01f && v3[3] == 0.0f &&
                                 v4[0] == 0.04502119805 && v4[1] == 0.0 &&
                                 v4[2] == -1.0/2.0});

    SG_PRINTVEC(v1);
    SG_PRINTVEC(v2);
    SG_PRINTVEC(v3);
    SG_PRINTVEC(v4);

    Vec3i v5{-1, 2, -3};
    Vec3i v6{10, 20, -30};
    Vec3i v7{100, -200, -300};
    Vec3i v8 = (v7 -= v6) += v5;
    auto v9 = v6 - v7;
    auto v10 = v5 + v6;
    auto v11 = v10 * -1;
    auto v12 = v10 / 2;

    a.push_back({L"-=, +=", v7[0] == 89 && v7[1] == -218 && v7[2] == -273 &&
                            v6[0] == 10 && v6[1] == 20 && v6[2] == -30 &&
                            v5[0] == -1 && v5[1] == 2 && v5[2] == -3});
    a.push_back({L"-, +", v9[0] == -79 && v9[1] == 238 && v9[2] == 243 &&
                          v10[0] == 9 && v10[1] == 22 && v10[2] == -33});
    a.push_back({L"*, /", v11[0] == -9 && v11[1] == -22 && v11[2] == 33 &&
                          v12[0] == 4 && v12[1] == 11 && v12[2] == -16});

    SG_PRINTVEC(v5);
    SG_PRINTVEC(v6);
    SG_PRINTVEC(v7);
    SG_PRINTVEC(v8);
    SG_PRINTVEC(v9);
    SG_PRINTVEC(v10);
    SG_PRINTVEC(v11);
    SG_PRINTVEC(v12);

    Vec4f v13{2.0f, 4.0f, 8.0f, 16.0f};
    Vec4f v14{6.0f, 5.0f, 4.0f, 3.0f};
    auto s1 = v13.length();
    auto s2 = v14.length();
    auto s3 = Vec3f{-3.0f, -2.0f, -6.0f}.length();
    v13.normalize();
    auto v15 = normalize(v14);
    auto s4 = v13.length();
    auto s5 = v14.length();
    auto s6 = v15.length();

    a.push_back({L"normalize, length", s3 == 7 && s4 == 1 && s5 != 1});

    SG_PRINTVEC(v13);
    SG_PRINTVEC(v14);
    SG_PRINTSCAL(s1);
    SG_PRINTSCAL(s2);
    SG_PRINTSCAL(s3);
    SG_PRINTVEC(v15);
    SG_PRINTSCAL(s4);
    SG_PRINTSCAL(s5);
    SG_PRINTSCAL(s6);

    Vec3f v16{1.0f, 0.0f, 0.0f};
    Vec3f v17{0.0f, 0.0f, 1.0f};
    auto v18 = cross(v16, v17);
    auto s7 = dot(Vec2d{5.0, -2.0}, Vec2d{0.25, 0.1});

    a.push_back({L"cross", v18[0] == 0 && v18[1] == -1 && v18[2] == 0});
    a.push_back({L"dot", s7 == 1.05});

    SG_PRINTVEC(v16);
    SG_PRINTVEC(v17);
    SG_PRINTVEC(v18);
    SG_PRINTSCAL(s7);

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::vectorTest() {
  static VectorTest test;
  return &test;
}
