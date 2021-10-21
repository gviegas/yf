//
// WS
// UnitTests.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_WS_UNITTESTS_H
#define YF_WS_UNITTESTS_H

#include <string>
#include <vector>
#include <functional>
#include <utility>
#include <algorithm>
#include <iostream>

#include "Test.h"

TEST_NS_BEGIN

Test* windowTest();
Test* eventTest();

using TestFn = std::function<Test* ()>;
using TestID = std::pair<std::string, std::vector<TestFn>>;
const std::vector<TestID> TIDs{
  TestID("window", {windowTest}),
  TestID("event", {eventTest}),
  TestID("all", {windowTest, eventTest})
};

inline std::vector<Test*> unitTests(const std::string& id) {
  auto it = std::find_if(TIDs.begin(), TIDs.end(),
                         [&](auto& p) { return p.first == id; });

  if (it == TIDs.end()) {
    wprintf(L"\n! Unknown test `%s` requested", id.data());
    wprintf(L"\n\nThe following tests are available:");
    for (const auto& kv : TIDs)
      wprintf(L"\n- %s", kv.first.data());
    return {};
  }

  std::vector<Test*> tests{};
  for (const auto& tf : it->second)
    tests.push_back(tf());
  return tests;
}

TEST_NS_END

#endif // YF_WS_UNITTESTS_H
