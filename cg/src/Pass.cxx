//
// CG
// Pass.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Pass.h"

using namespace CG_NS;
using namespace std;

Pass::Pass(const vector<ColorAttach>* colors,
           const vector<ColorAttach>* resolves,
           const DepStenAttach* depthStencil)
           : colors_(colors ?
                     make_unique<vector<ColorAttach>>(*colors) :
                     nullptr),
             resolves_(resolves ?
                       make_unique<vector<ColorAttach>>(*resolves) :
                       nullptr),
             depthStencil_(depthStencil ?
                           make_unique<DepStenAttach>(*depthStencil) :
                           nullptr) { }

Pass::~Pass() { }

Target::Target(Size2 size,
               uint32_t layers,
               const std::vector<AttachImg>* colors,
               const std::vector<AttachImg>* resolves,
               const AttachImg* depthStencil)
               : size_(size), layers_(layers),
                 colors_(colors ?
                         make_unique<vector<AttachImg>>(*colors) :
                         nullptr),
                 resolves_(resolves ?
                           make_unique<vector<AttachImg>>(*resolves) :
                           nullptr),
                 depthStencil_(depthStencil ?
                               make_unique<AttachImg>(*depthStencil) :
                               nullptr) { }

Target::~Target() { }
