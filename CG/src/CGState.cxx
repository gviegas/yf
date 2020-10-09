//
// yf
// CGState.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include "CGState.h"

using namespace YF_NS;

CGGraphState::CGGraphState(const Config& config) : config(config) {}
CGGraphState::CGGraphState(Config&& config) : config(config) {}
CGGraphState::~CGGraphState() {}

CGCompState::CGCompState(const Config& config) : config(config) {}
CGCompState::CGCompState(Config&& config) : config(config) {}
CGCompState::~CGCompState() {}
