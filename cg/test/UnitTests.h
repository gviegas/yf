//
// CG
// UnitTests.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_UNITTESTS_H
#define YF_CG_UNITTESTS_H

#include <cstring>

#include "Test.h"

TEST_NS_BEGIN

constexpr const char* TestIdDft = "";
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

constexpr const char* TestIdDraw = "draw";
Test* drawTest();

inline std::vector<Test*> unitTests(const std::string& id) {
  if (strcmp(id.data(), TestIdDraw) == 0)
    return {drawTest()};

  return {typesTest(), deviceTest(), queueTest(), bufferTest(),
          imageTest(), shaderTest(), dcTableTest(), passTest(),
          stateTest(), encoderTest(), wsiTest()};
}

TEST_NS_END

#endif // YF_CG_UNITTESTS_H
