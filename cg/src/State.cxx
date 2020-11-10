//
// CG
// State.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/cg/State.h"

using namespace CG_NS;

GrState::GrState(const Config& config) : config_(config) { }
GrState::~GrState() { }

CpState::CpState(const Config& config) : config_(config) { }
CpState::~CpState() { }
