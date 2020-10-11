//
// yf
// CGState.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "CGState.h"

using namespace YF_NS;

CGGrState::CGGrState(const Config& config) : config(config) {}
CGGrState::CGGrState(Config&& config) : config(config) {}
CGGrState::~CGGrState() {}

CGCpState::CGCpState(const Config& config) : config(config) {}
CGCpState::CGCpState(Config&& config) : config(config) {}
CGCpState::~CGCpState() {}
