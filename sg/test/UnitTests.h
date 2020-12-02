//
// SG
// UnitTests.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_UNITTESTS_H
#define YF_SG_UNITTESTS_H

#include "Test.h"

TEST_NS_BEGIN

inline const std::vector<Test*>& unitTests() {
  static std::vector<Test*> tests{};
  return tests;
}

TEST_NS_END

#endif // YF_SG_UNITTESTS_H
