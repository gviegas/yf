//
// yf
// UnitTests.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_UNITTESTS_H
#define YF_UNITTESTS_H

#include "Test.h"

TEST_NS_BEGIN

Test* typesTest();
Test* resultTest();
Test* deviceTest();
Test* bufferTest();
Test* imageTest();
Test* shaderTest();
Test* dcTableTest();
Test* passTest();
Test* stateTest();
Test* encoderTest();
Test* queueTest();

inline const std::vector<Test*> unitTests() {
  static std::vector<Test*> tests{
    typesTest(), resultTest(), deviceTest(), bufferTest(), imageTest(),
    shaderTest(), dcTableTest(), passTest(), stateTest(), encoderTest(),
    queueTest()};
  return tests;
}

TEST_NS_END

#endif // YF_UNITTESTS_H
