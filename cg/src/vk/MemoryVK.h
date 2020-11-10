//
// CG
// MemoryVK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_MEMORYVK_H
#define YF_CG_MEMORYVK_H

#include "Defs.h"
#include "VK.h"

CG_NS_BEGIN

/// Allocates device memory.
///
VkDeviceMemory allocateVK(const VkMemoryRequirements& requirements,
                          bool hostVisible);

/// Deallocates device memory.
///
void deallocateVK(VkDeviceMemory memory);

CG_NS_END

#endif // YF_CG_MEMORYVK_H
