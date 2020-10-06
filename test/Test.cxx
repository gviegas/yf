//
// yf
// Test.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include <iostream>

#include "Test.h"

using namespace TEST_NS;
using namespace std;

Test::Test(wstring&& name) : name(name) {}

Test::~Test() {}

Coverage TEST_NS::run(const vector<Test*>& tests, vector<string>&& args)  {
  if (tests.size() == 0) {
    wcout << "No tests to run\n";
    return 0.0;
  }

  const auto n = tests.size();
  Coverage unit, total = 0.0;

  for (auto test : tests) {
    wcout << "\n**** Running test '" << test->name << "' ****\n";
    unit = test->run(args);
    total += unit;
    wstring dots;
    dots.resize(32*unit);
    fill(dots.begin(), dots.end(), L'.');
    wcout << "\n" << dots << " " << unit*100.0 << "%\n\n";
  }

  total /= n;
  wcout << "\n- Test coverage : " << total*100.0 << "%"
        << "\n- Unit tests run: " << n << endl;

  return total;
}
