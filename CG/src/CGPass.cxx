//
// yf
// CGPass.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include "CGPass.h"

using namespace YF_NS;
using namespace std;

CGPass::CGPass(const vector<CGColorAttach>* colors,
               const vector<CGColorAttach>* resolves,
               const CGDepStenAttach* depthStencil)
               : colors(colors != nullptr ?
                        make_unique<vector<CGColorAttach>>(*colors) :
                        nullptr),
                 resolves(resolves != nullptr ?
                          make_unique<vector<CGColorAttach>>(*resolves) :
                          nullptr),
                 depthStencil(depthStencil != nullptr ?
                              make_unique<CGDepStenAttach>(*depthStencil) :
                              nullptr) {}

CGPass::~CGPass() {}
