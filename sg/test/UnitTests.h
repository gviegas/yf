//
// SG
// UnitTests.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_UNITTESTS_H
#define YF_SG_UNITTESTS_H

#include <cstring>

#include "Test.h"

TEST_NS_BEGIN

constexpr const char* TestIdDft = "";
Test* nodeTest();
Test* sceneTest();
Test* viewTest();
Test* vectorTest();
Test* matrixTest();
Test* meshTest();
Test* textureTest();
Test* cameraTest();

constexpr const char* TestIdCam = "camera";
Test* camTest();

constexpr const char* TestIdMisc = "misc";
Test* miscTest();

inline std::vector<Test*> unitTests(const std::string& id) {
  if (strcmp(id.data(), TestIdMisc) == 0)
    return {miscTest()};

  if (strcmp(id.data(), TestIdCam) == 0)
    return {cameraTest()};

  return {nodeTest(), sceneTest(), viewTest(),
          vectorTest(), matrixTest(),
          meshTest(), textureTest()};
}

TEST_NS_END

#endif // YF_SG_UNITTESTS_H
