#pragma once

#include "Defines.h"

struct PlatformState;
struct VulkanContext;

b8 PlatformCreateVulkanSurface(struct PlatformState* _state, struct VulkanContext* _context);

/**
 * Appends the names of the required extensions for this platform to
 * namesDArray which will be created then passed in
*/
void PlatformGetRequiredExtensionNames(const char*** _namesDArray);