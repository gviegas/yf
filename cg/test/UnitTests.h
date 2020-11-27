//
// CG
// UnitTests.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_UNITTESTS_H
#define YF_CG_UNITTESTS_H

#include "Test.h"

TEST_NS_BEGIN

#if defined(YF_CG_TEST_API)
# define unitTests unitTests0
#else
# define unitTests unitTests1
#endif

Test* typesTest();
Test* deviceTest();
Test* queueTest();
Test* bufferTest();
Test* imageTest();
Test* shaderTest();
Test* dcTableTest();
Test* passTest();
Test* stateTest();
Test* encoderTest();
Test* wsiTest();

inline const std::vector<Test*>& unitTests0() {
  static std::vector<Test*> tests{
    typesTest(), deviceTest(), queueTest(), bufferTest(),
    imageTest(), shaderTest(), dcTableTest(), passTest(),
    stateTest(), encoderTest(), wsiTest()};

  return tests;
}

Test* drawTest();

inline const std::vector<Test*>& unitTests1() {
  static std::vector<Test*> tests{drawTest()};
  return tests;
}

TEST_NS_END

#endif // YF_CG_UNITTESTS_H
