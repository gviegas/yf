//
// CG
// UnitTests.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_UNITTESTS_H
#define YF_CG_UNITTESTS_H

#include <string>
#include <vector>
#include <functional>
#include <utility>
#include <algorithm>
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
Test* limitsTest();
Test* drawTest();
Test* copyTest();

using TestFn = std::function<Test* ()>;
using TestID = std::pair<std::string, std::vector<TestFn>>;
const std::vector<TestID> TIDs{
  TestID("types", {typesTest}),
  TestID("device", {deviceTest}),
  TestID("queue", {queueTest}),
  TestID("buffer", {bufferTest}),
  TestID("image", {imageTest}),
  TestID("shader", {shaderTest}),
  TestID("dctable", {dcTableTest}),
  TestID("pass", {passTest}),
  TestID("state", {stateTest}),
  TestID("encoder", {encoderTest}),
  TestID("wsi", {wsiTest}),
  TestID("limits", {limitsTest}),
  TestID("draw", {drawTest}),
  TestID("copy", {copyTest}),
  TestID("all", {typesTest, deviceTest, queueTest, bufferTest, imageTest,
                 shaderTest, dcTableTest, passTest, stateTest, encoderTest,
                 wsiTest, limitsTest, drawTest, copyTest})
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

#endif // YF_CG_UNITTESTS_H
