//
// SG
// AnimationTest.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "Node.h"
#include "Animation.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct AnimationTest : Test {
  AnimationTest() : Test(L"Animation") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    vector<Animation::Timeline> inputs{{0.5f, 1.0f}, {0.33f, 0.67f, 1.0f}};
    vector<Animation::Translation> outT{
      {Vec3f{1.0f, 1.0f, 1.0f}, Vec3f{0.0f, 0.0f, 2.0f}},
      {Vec3f{2.0f, 4.0f, 0.0f}, Vec3f{5.0f}, Vec3f{-3.0f}}};

    Animation an1(inputs, outT, {}, {});

    a.push_back({L"Animation(...)", an1.actions().empty() &&
                                    an1.inputs().size() == 2 &&
                                    an1.outT().size() == 2 &&
                                    an1.outR().empty() && an1.outS().empty()});

    Animation an2;

    a.push_back({L"Animation()", !an2});

    Animation an3(an1);
    Animation an4 = an3;

    a.push_back({L"Animation(other), =", an3.actions().empty() &&
                                         an4.actions().empty() &&
                                         an3.inputs().size() == 2 &&
                                         an4.inputs().size() == 2 &&
                                         an3.outT().size() == 2 &&
                                         an4.outT().size() == 2 &&
                                         an3.outR().empty() &&
                                         an4.outR().empty() &&
                                         an3.outS().empty() &&
                                         an4.outS().empty()});

    Node nd;
    Animation::Action act{&nd, Animation::T, Animation::Step, 1, 1};
    an1.actions().push_back(act);

    a.push_back({L"actions()", an1.actions().size() == 1 &&
                               an3.actions().size() == 1 &&
                               an1.actions().front().input == 1 &&
                               an4.actions().front().output == 1});

    a.push_back({L"bool, !", an1 && !an2 && an3 && an4 && !Animation(an2)});

    a.push_back({L"==, !=", an1 == an3 && an4 == an1 && an1 != an2 &&
                            an2 == Animation()});

    an1.name() = L"an1";

    a.push_back({L"name()", an1.name() == L"an1" && an1.name() == an4.name()});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* animationTest() {
  static AnimationTest test;
  return &test;
}

TEST_NS_END
