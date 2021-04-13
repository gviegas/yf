//
// CG
// UnitTests.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_UNITTESTS_H
#define YF_CG_UNITTESTS_H

#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

#include "Test.h"

TEST_NS_BEGIN

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
Test* drawTest();
Test* copyTest();

using TestFn = std::function<Test* ()>;
const std::unordered_map<std::string, std::vector<TestFn>> TIDs{
  {"types", {typesTest}},
  {"device", {deviceTest}},
  {"queue", {queueTest}},
  {"buffer", {bufferTest}},
  {"image", {imageTest}},
  {"shader", {shaderTest}},
  {"dctable", {dcTableTest}},
  {"pass", {passTest}},
  {"state", {stateTest}},
  {"encoder", {encoderTest}},
  {"wsi", {wsiTest}},
  {"draw", {drawTest}},
  {"copy", {copyTest}},
  {"all", {typesTest, deviceTest, queueTest, bufferTest, imageTest, shaderTest,
           dcTableTest, passTest, stateTest, encoderTest, wsiTest, drawTest,
           copyTest}}
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

#endif // YF_CG_UNITTESTS_H
