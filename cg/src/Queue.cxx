//
// CG
// Queue.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Queue.h"

using namespace CG_NS;

Queue::Queue(CapabilityMask capabilities) : capabilities_(capabilities) { }
Queue::~Queue() { }

CmdBuffer::~CmdBuffer() { }
