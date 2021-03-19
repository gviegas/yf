//
// SG
// MatrixTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cstring>
#include <iostream>

#include "UnitTests.h"
#include "Matrix.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct MatrixTest : Test {
  MatrixTest() : Test(L"Matrix") { }

#define SG_PRINTVEC(vec) do { \
  wcout << endl << #vec ":\n"; \
  for (const auto& x : vec) \
    wcout << "   " << x << endl; \
} while (false)

#define SG_PRINTMAT(mat) do { \
  wcout << endl << #mat ":"; \
  for (size_t i = 0; i < mat.rows(); ++i) { \
    wcout << "\n   "; \
    for (size_t j = 0; j < mat.columns(); ++j) \
      wcout << mat[j][i] << "\t\t"; \
  } \
  wcout << endl; \
} while (false)

  Assertions run(const vector<string>&) {
    Assertions a;
    wcout.setf(ios_base::fixed);
    auto prec = wcout.precision(1);

    Mat3x2i m0{{10, -20}, {-30, 40}, {-512 , -256}};

    a.push_back({L"Matrix(...)", m0[0][0] == 10 && m0[0][1] == -20 &&
                                 m0[1][0] == -30 && m0[1][1] == 40 &&
                                 m0[2][0] == -512 && m0[2][1] == -256});

    Mat2i m1{{1, 2}, {3, 4}};
    Mat2i m2{{-2, -3}, {-4, -5}};
    auto m3 = (m1 -= m2) += {{2, 2}, {3, 3}};
    auto m4 = m1-m2;
    auto m5 = m2-m1;
    auto m6 = m1+m2;

    a.push_back({L"-=, +=", m1[0][0] == 5 && m1[0][1] == 7 &&
                            m1[1][0] == 10 && m1[1][1] == 12 &&
                            m2[0][0] == -2 && m2[0][1] == -3 &&
                            m2[1][0] == -4 && m2[1][1] == -5 &&
                            m3[0][0] == m1[0][0] && m3[0][1] == m1[0][1] &&
                            m3[1][0] == m1[1][0] && m3[1][1] == m1[1][1]});
    a.push_back({L"-, +", m4[0][0] == 7 && m4[0][1] == 10 &&
                          m4[1][0] == 14 && m4[1][1] == 17 &&
                          m5[0][0] == -7 && m5[0][1] == -10 &&
                          m5[1][0] == -14 && m5[1][1] == -17 &&
                          m6[0][0] == 3 && m6[0][1] == 4 &&
                          m6[1][0] == 6 && m6[1][1] == 7});

    SG_PRINTMAT(m1);
    SG_PRINTMAT(m2);
    SG_PRINTMAT(m3);
    SG_PRINTMAT(m4);
    SG_PRINTMAT(m5);
    SG_PRINTMAT(m6);

    Mat2i m7{{1, 0}, {4, 1}};
    Mat2i m8{{3, 0}, {0, 1}};
    auto m9 = m7 * m8;
    auto m10 = m9;
    m10 *= m7;

    a.push_back({L"*, *=", m7[0][0] == 1 && m7[0][1] == 0 &&
                           m7[1][0] == 4 && m7[1][1] == 1 &&
                           m8[0][0] == 3 && m8[0][1] == 0 &&
                           m8[1][0] == 0 && m8[1][1] == 1 &&
                           m9[0][0] == 3 && m9[0][1] == 0 &&
                           m9[1][0] == 4 && m9[1][1] == 1 &&
                           m10[0][0] == 3 && m10[0][1] == 0 &&
                           m10[1][0] == 16 && m10[1][1] == 1});

    SG_PRINTMAT(m7);
    SG_PRINTMAT(m8);
    SG_PRINTMAT(m9);
    SG_PRINTMAT(m10);

    Mat4f m11{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {5, 10, 2, 1}};
    Vec4f v1{{2, 3, 4, 1}};
    auto v2 = m11 * v1;

    a.push_back({L"* vec", v2[0] == 7 && v2[1] == 13 &&
                           v2[2] == 6 && v2[3] == 1});

    SG_PRINTMAT(m11);
    SG_PRINTVEC(v1);
    SG_PRINTVEC(v2);

    auto m12 = Mat2i::identity();
    auto m13 = Mat3d::identity();
    auto m14 = Mat4f::identity();

    a.push_back({L"identity()", [&]() {
      for (size_t i = 0; i < m12.columns(); ++i) {
        if (m12[i][i] != 1) return false;
      }
      for (size_t i = 0; i < m13.columns(); ++i) {
        if (m13[i][i] != 1) return false;
      }
      for (size_t i = 0; i < m14.columns(); ++i) {
        if (m14[i][i] != 1) return false;
      }
      return true;
    }() });

    SG_PRINTMAT(m12);
    SG_PRINTMAT(m13);
    SG_PRINTMAT(m14);

    Mat2i m15{{1, 2}, {4, 5}};
    auto m16 = m15;
    m16.transpose();
    auto m17 = transpose(m15);

    a.push_back({L"transpose()", m15[0][0] == 1 && m15[0][1] == 2 &&
                                 m15[1][0] == 4 && m15[1][1] == 5 &&
                                 m16[0][0] == 1 && m16[0][1] == 4 &&
                                 m16[1][0] == 2 && m16[1][1] == 5 &&
                                 m17[0][0] == m16[0][0] &&
                                 m17[0][1] == m16[0][1] &&
                                 m17[1][0] == m16[1][0] &&
                                 m17[1][1] == m16[1][1]});

    SG_PRINTMAT(m15);
    SG_PRINTMAT(m16);
    SG_PRINTMAT(m17);

    auto m18 = invert(Mat2d{{12.0, 0.0}, {-1.0, 4.0}});
    auto m19 = invert(Mat3d{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {7.0, 8.0, 9.0}});
    auto m20 = invert(Mat4d{{-2.0, 0.0, 0.0, 0.0}, {0.0, -34.0, 0.0, 1.0},
                            {0.0, 0.0, -1.0, 2.0}, {0.0, 1.0, 2.0, -16.0}});
    auto m21 = invert(m18);
    auto m22 = invert(m19);
    auto m23 = invert(m20);

    SG_PRINTMAT(m18);
    SG_PRINTMAT(m19);
    SG_PRINTMAT(m20);
    SG_PRINTMAT(m21);
    SG_PRINTMAT(m22);
    SG_PRINTMAT(m23);

    auto m24 = rotate(3.141592, {0.0, -1.0, 0.0});
    auto m25 = rotateX(-3.141592);
    auto m26 = rotateY(-3.141592);
    auto m27 = rotateZ(-3.141592);
    auto m28 = scale(0.5, 0.2, 12.75);
    auto m29 = translate(12, 2, -20);

    SG_PRINTMAT(m24);
    SG_PRINTMAT(m25);
    SG_PRINTMAT(m26);
    SG_PRINTMAT(m27);
    SG_PRINTMAT(m28);
    SG_PRINTMAT(m29);

    auto m30 = lookAt(Vec3f{0.0, -5.0, -12.0}, Vec3f{0.0, 0.0, 0.0},
                      Vec3f{0.0, -1.0, 0.0});
    auto m31 = perspective(1.7172, 1.6667, 0.01, 100.0);
    auto m32 = infPerspective(1.7172, 1.6667, 0.01);
    auto m33 = ortho(1.0, 1.0, 0.0, -1.0);

    SG_PRINTMAT(m30);
    SG_PRINTMAT(m31);
    SG_PRINTMAT(m32);
    SG_PRINTMAT(m33);

    bool check = true;

    auto m34 = Mat4f::identity();
    m34[0] = {1.0f, 2.0f, 3.0f, 4.0f};
    m34[1][3] = 0xff;
    m34[2] = {10.0f, 20.0f, 30.0f, 40.0f};
    m34[3][3] = -0xff;

    SG_PRINTMAT(m34);

    float b[16];
    memcpy(b, m34.data(), sizeof b);
    if (memcmp(b, m34.data(), sizeof b) != 0)
      check = false;

    b[10] = 5.5f;
    m34.data()[2] = -0.263f;
    if (memcmp(b, m34.data(), sizeof b) == 0)
      check = false;

    b[0] *= -1.0f;
    b[15] += 248.0f;
    memcpy(m34.data(), b, sizeof b);
    if (memcmp(m34.data(), b, sizeof b) != 0)
      check = false;

    SG_PRINTMAT(m34);

    auto m35 = rotate3(rotateQ(M_PI_4, {0.0, 0.0, 1.0}));
    auto m36 = rotate(rotateQ(M_PI_4, {0.0, 0.0, -1.0}));

    SG_PRINTMAT(m35);
    SG_PRINTMAT(m36);

    a.push_back({L"data()", check});

    wcout.precision(prec);
    wcout.unsetf(ios_base::fixed);
    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::matrixTest() {
  static MatrixTest test;
  return &test;
}
