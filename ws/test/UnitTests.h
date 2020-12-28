//
// WS
// UnitTests.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_WS_UNITTESTS_H
#define YF_WS_UNITTESTS_H

#include <cstring>

#include "Test.h"

TEST_NS_BEGIN

constexpr const char* TestIdDft = "";
Test* windowTest();
Test* eventTest();

inline std::vector<Test*> unitTests(const std::string& id) {
  return {windowTest(), eventTest()};
}

TEST_NS_END

#endif // YF_WS_UNITTESTS_H
