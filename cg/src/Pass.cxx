//
// CG
// Pass.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/cg/Pass.h"

using namespace CG_NS;
using namespace std;

Pass::Pass(const vector<ColorAttach>* colors,
           const vector<ColorAttach>* resolves,
           const DepStenAttach* depthStencil)
           : colors(colors != nullptr ?
                    make_unique<vector<ColorAttach>>(*colors) :
                    nullptr),
             resolves(resolves != nullptr ?
                      make_unique<vector<ColorAttach>>(*resolves) :
                      nullptr),
             depthStencil(depthStencil != nullptr ?
                          make_unique<DepStenAttach>(*depthStencil) :
                          nullptr) {}

Pass::~Pass() {}
