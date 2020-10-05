//
// yf
// Run.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include <iostream>

#include "UnitTests.h"

using namespace TEST_NS;
using namespace std;

int main(int argc, char* argv[]) {
  wcout << "[yf-CG] Test\n------------\n\n";
  for (int i = 0; i < argc; ++i)
    wcout << argv[i] << " ";
  wcout << endl;

  // TODO: Argument list.
  run(unitTests());

  wcout << "\n-----------\nEnd of test\n";
}
