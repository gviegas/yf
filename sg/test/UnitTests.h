//
// SG
// UnitTests.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_SG_UNITTESTS_H
#define YF_SG_UNITTESTS_H

#include <cstring>

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

constexpr const char* TestIdCam = "camera";
Test* cameraTest();

constexpr const char* TestIdMisc = "misc";
Test* miscTest();

inline std::vector<Test*> unitTests(const std::string& id) {
  if (strcmp(id.data(), TestIdMisc) == 0)
    return {miscTest()};

  if (strcmp(id.data(), TestIdCam) == 0)
    return {cameraTest()};

  return {nodeTest(), sceneTest(), viewTest(),
          vectorTest(), quaternionTest(), matrixTest(),
          meshTest(), textureTest()};
}

TEST_NS_END

#endif // YF_SG_UNITTESTS_H
