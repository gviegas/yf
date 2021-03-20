//
// WS
// UnitTests.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_WS_UNITTESTS_H
#define YF_WS_UNITTESTS_H

#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

#include "Test.h"

TEST_NS_BEGIN

Test* windowTest();
Test* eventTest();

using TestFn = std::function<Test* ()>;
const std::unordered_map<std::string, std::vector<TestFn>> TIDs{
  {"window", {windowTest}},
  {"event", {eventTest}},
  {"all", {windowTest, eventTest}}
};

inline std::vector<Test*> unitTests(const std::string& id) {
  auto it = TIDs.find(id);

  if (it == TIDs.end()) {
    wprintf(L"\n! Unknown test `%s` requested", id.data());
    return {};
  }

  std::vector<Test*> tests{};
  for (const auto& tf : it->second)
    tests.push_back(tf());
  return tests;
}

TEST_NS_END

#endif // YF_WS_UNITTESTS_H
