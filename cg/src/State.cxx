//
// CG
// State.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "State.h"

using namespace CG_NS;

GrState::GrState(const Config& config) : config_(config) { }
GrState::~GrState() { }

CpState::CpState(const Config& config) : config_(config) { }
CpState::~CpState() { }
