//
// yf
// CGQueue.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "CGQueue.h"

using namespace YF_NS;
using namespace std;

CGQueue::CGQueue(CapabilityMask capabilities) : capabilities(capabilities) {}
CGQueue::~CGQueue() {}

CGCmdBuffer::~CGCmdBuffer() {}
