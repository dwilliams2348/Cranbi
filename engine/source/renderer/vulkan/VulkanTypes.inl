#pragma once

#include "Defines.h"
#include "core/Asserts.h"

#include <vulkan/vulkan.h>

//checks given expression and returns against VK_SUCCESS
#define VK_CHECK(_expr)                     \
    {                                       \
        CORE_ASSERT(_expr == VK_SUCCESS);   \
    }

typedef struct VulkanContext
{
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    
#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debugMessenger;
#endif
} VulkanContext;