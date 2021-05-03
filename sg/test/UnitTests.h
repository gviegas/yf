//
// SG
// UnitTests.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_SG_UNITTESTS_H
#define YF_SG_UNITTESTS_H

#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
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
Test* modelTest();
Test* collectionTest();
Test* cameraTest();
Test* renderTest();
Test* miscTest();

using TestFn = std::function<Test* ()>;
const std::unordered_map<std::string, std::vector<TestFn>> TIDs{
  {"node", {nodeTest}},
  {"scene", {sceneTest}},
  {"view", {viewTest}},
  {"vector", {vectorTest}},
  {"quaternion", {quaternionTest}},
  {"matrix", {matrixTest}},
  {"mesh", {meshTest}},
  {"texture", {textureTest}},
  {"material", {materialTest}},
  {"model", {modelTest}},
  {"collection", {collectionTest}},
  {"camera", {cameraTest}},
  {"render", {renderTest}},
  {"misc", {miscTest}},
  {"all", {nodeTest, sceneTest, viewTest, vectorTest, quaternionTest,
           matrixTest, meshTest, textureTest, materialTest, modelTest,
           collectionTest, cameraTest, renderTest, miscTest}}
};

inline std::vector<Test*> unitTests(const std::string& id) {
  auto it = TIDs.find(id);

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
