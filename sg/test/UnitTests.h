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
Test* cameraTest();
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
  {"camera", {cameraTest}},
  {"misc", {miscTest}},
  {"all", {nodeTest, viewTest, vectorTest, quaternionTest, matrixTest,
           meshTest, textureTest, cameraTest, miscTest}}
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

#endif // YF_SG_UNITTESTS_H
