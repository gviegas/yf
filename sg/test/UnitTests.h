//
// SG
// UnitTests.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_SG_UNITTESTS_H
#define YF_SG_UNITTESTS_H

#include <string>
#include <vector>
#include <functional>
#include <utility>
#include <algorithm>
#include <iostream>

#include "Test.h"

TEST_NS_BEGIN

Test* nodeTest();
Test* sceneTest();
Test* viewTest();
Test* vectorTest();
Test* quaternionTest();
Test* matrixTest();
Test* meshTest();
Test* textureTest();
Test* materialTest();
Test* skinTest();
Test* modelTest();
Test* animationTest();
Test* collectionTest();
Test* cameraTest();
Test* bodyTest();
Test* renderTest();

using TestFn = std::function<Test* ()>;
using TestID = std::pair<std::string, std::vector<TestFn>>;
const std::vector<TestID> TIDs{
  TestID("node", {nodeTest}),
  TestID("scene", {sceneTest}),
  TestID("view", {viewTest}),
  TestID("vector", {vectorTest}),
  TestID("quaternion", {quaternionTest}),
  TestID("matrix", {matrixTest}),
  TestID("mesh", {meshTest}),
  TestID("texture", {textureTest}),
  TestID("material", {materialTest}),
  TestID("skin", {skinTest}),
  TestID("model", {modelTest}),
  TestID("animation", {animationTest}),
  TestID("collection", {collectionTest}),
  TestID("camera", {cameraTest}),
  TestID("body", {bodyTest}),
  TestID("render", {renderTest}),
  TestID("all", {nodeTest, sceneTest, viewTest, vectorTest, quaternionTest,
                 matrixTest, meshTest, textureTest, materialTest, skinTest,
                 modelTest, animationTest, collectionTest, cameraTest,
                 bodyTest, renderTest})
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

#endif // YF_SG_UNITTESTS_H
