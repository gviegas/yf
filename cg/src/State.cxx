//
// cg
// State.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/cg/State.h"

using namespace CG_NS;

GrState::GrState(const Config& config) : config(config) {}
GrState::GrState(Config&& config) : config(config) {}
GrState::~GrState() {}

CpState::CpState(const Config& config) : config(config) {}
CpState::CpState(Config&& config) : config(config) {}
CpState::~CpState() {}
