//
// SG
// QuaternionTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <iostream>

#include "Test.h"
#include "Quaternion.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct QuaternionTest : Test {
  QuaternionTest(): Test(L"Quaternion") { }

#define SG_PRINTQNION(qnion) do { \
  wcout << endl << #qnion ":\n"; \
  wcout << "   r = " << qnion.r() << "\n   v = " << qnion.v()[0] << ", " \
        << qnion.v()[1] << ", " << qnion.v()[2] << endl; \
} while (0)

  Assertions run(const vector<string>&) {
    Assertions a;

    Quaternion<double> q1;
    SG_PRINTQNION(q1);

    Quaternion<double> q2{1.0, {0.7071, 0.0, -0.7071}};
    SG_PRINTQNION(q2);

    Quaternion<double> q3{Vec4d{0.1, 0.2, 0.3, 0.4}};
    SG_PRINTQNION(q3);

    q3.r() *= 2.0;
    q3.v()[1] /= 4.0;

    Quaternion q4{q3.q()};
    SG_PRINTQNION(q4);

    a.push_back({L"Quaternion()", q1.r() == 0 && q1.v()[0] == 0 &&
                                  q1.v()[1] == 0 && q1.v()[2] == 0 &&
                                  q2.r() == 1 && q2.v()[0] > 0 &&
                                  q2.v()[1] == 0 && q2.v()[2] < 0 &&
                                  q3.r() == 0.8 && q3.v()[0] == 0.1 &&
                                  q3.v()[1] == 0.05 && q3.v()[2] == 0.3});

    Quaternion<double> q5{M_PI, {1.0, 0.0, 0.0}};
    Quaternion<double> q6{M_PI, {0.0, 0.0, -1.0}};
    SG_PRINTQNION(q5);
    SG_PRINTQNION(q6);

    auto q7 = q6 * q5;
    auto q8 = q5 * q6;
    q6 *= q5;
    SG_PRINTQNION(q5);
    SG_PRINTQNION(q6);
    SG_PRINTQNION(q7);
    SG_PRINTQNION(q8);

    a.push_back({L"*, *=", q7.r() == q6.r() && q7.v()[0] == q6.v()[0] &&
                           q7.v()[1] == q6.v()[1] && q7.v()[2] == q6.v()[2] &&
                           fabs(q7.r() - 9.8696) < 0.00001 &&
                           fabs(q7.v()[0] - M_PI) < 0.0000000001 &&
                           q7.v()[1] == -1 &&
                           fabs(q7.v()[2] + M_PI) < 0.0000000001});

    auto q9 = rotateQ(M_PI_2, {1.0});
    auto q10 = rotateQ(M_PI_2, {0.0, 1.0});
    auto q11 = q9 * q10;
    SG_PRINTQNION(q9);
    SG_PRINTQNION(q10);
    SG_PRINTQNION(q11);

    a.push_back({L"rotateQ", fabs(q11.r() - 0.5) < 0.00000000001 &&
                             fabs(q11.v()[0] - 0.5) < 0.0000000001 &&
                             fabs(q11.v()[1] - 0.5) < 0.0000000001 &&
                             fabs(q11.v()[2] - 0.5) < 0.0000000001});

    auto q12 = rotateQX(-M_PI_4);
    auto q13 = rotateQY(-M_PI_4);
    auto q14 = rotateQZ(-M_PI_4);
    SG_PRINTQNION(q12);
    SG_PRINTQNION(q13);
    SG_PRINTQNION(q14);

    a.push_back({L"rotateQ{XYZ}", q12.r() == q13.r() && q13.r() == q14.r() &&
                                  q12.v()[0] == q13.v()[1] &&
                                  q13.v()[1] == q14.v()[2] &&
                                  q12.v()[1] == 0 && q12.v()[2] == 0 &&
                                  q13.v()[0] == 0 && q13.v()[2] == 0 &&
                                  q14.v()[0] == 0 && q14.v()[1] == 0});

    wcout << "\nQnion sizes:\n flt: " << sizeof(Qnionf)
          << "\n dbl: " << sizeof(Qniond) << endl;
    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* quaternionTest() {
  static QuaternionTest test;
  return &test;
}

TEST_NS_END
