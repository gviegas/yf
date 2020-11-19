//
// WS
// UnitTests.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_WS_UNITTESTS_H
#define YF_WS_UNITTESTS_H

#include "Test.h"

TEST_NS_BEGIN

inline const std::vector<Test*>& unitTests() {
  static std::vector<Test*> tests{};

  return tests;
}

TEST_NS_END

#endif // YF_WS_UNITTESTS_H
