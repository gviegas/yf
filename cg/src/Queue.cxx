//
// cg
// Queue.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/cg/Queue.h"

using namespace CG_NS;

Queue::Queue(CapabilityMask capabilities) : capabilities(capabilities) {}
Queue::~Queue() {}

CmdBuffer::~CmdBuffer() {}
