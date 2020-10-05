//
// yf
// UnitTests.h
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_UNIT_TESTS_H
#define YF_CG_UNIT_TESTS_H

#include "Test.h"

TEST_NS_BEGIN

Test* resultTest();
Test* bufferTest();
Test* imageTest();

inline const std::vector<Test*> unitTests() {
  static std::vector<Test*> tests{resultTest(), bufferTest(), imageTest()};
  return tests;
}

TEST_NS_END

#endif // YF_CG_UNIT_TESTS_H
