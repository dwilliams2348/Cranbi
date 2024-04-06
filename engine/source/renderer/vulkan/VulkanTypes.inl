#pragma once

#include "Defines.h"
#include "core/Asserts.h"

#include <vulkan/vulkan.h>

//checks given expression and returns against VK_SUCCESS
#define VK_CHECK(_expr)                     \
    {                                       \
        CORE_ASSERT(_expr == VK_SUCCESS);   \
    }

typedef struct VulkanSwapchainSupportInfo
{
    VkSurfaceCapabilitiesKHR capabilities;
    u32 formatCount;
    VkSurfaceFormatKHR* formats;
    u32 presentModeCount;
    VkPresentModeKHR* presentModes;
} VulkanSwapchainSupportInfo;

typedef struct VulkanDevice
{
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    VulkanSwapchainSupportInfo swapchainSupport;
    i32 graphicsQueueIndex;
    i32 presentQueueIndex;
    i32 transferQueueIndex;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;

    VkFormat depthFormat;
} VulkanDevice;

typedef struct VulkanImage
{
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    u32 width;
    u32 height;
} VulkanImage;

typedef enum VulkanRenderpassState
{
    READY,
    RECORDING,
    IN_RENDER_PASS,
    RECORDING_ENDED,
    SUBMITTED,
    NOT_ALLOCATED
} VulkanRenderpassState;

typedef struct VulkanRenderpass
{
    VkRenderPass handle;
    f32 x, y, w, h;
    f32 r, g, b, a;
    f32 depth;
    u32 stencil;

    VulkanRenderpassState state;
} VulkanRenderpass;

typedef struct VulkanSwapchain
{
    VkSurfaceFormatKHR imageFormat;
    u8 maxFramesInFlight;
    VkSwapchainKHR handle;
    u32 imageCount;
    VkImage* images;
    VkImageView* views;

    VulkanImage depthAttachment;
} VulkanSwapchain;

typedef enum VulkanCommandBufferState
{
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDER_PASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,
    COMMAND_BUFFER_STATE_NOT_ALLOCATED
} VulkanCommandBufferState;

typedef struct VulkanCommandBuffer
{
    VkCommandBuffer handle;

    //command buffer state
    VulkanCommandBufferState state;
} VulkanCommandBuffer;

typedef struct VulkanContext
{
    //the framebuffers current width and height
    u32 framebufferWidth;
    u32 framebufferHeight;

    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
    
#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debugMessenger;
#endif

    VulkanDevice device;

    VulkanSwapchain swapchain;
    VulkanRenderpass mainRenderpass;

    u32 imageIndex;
    u32 currentFrame;

    b8 recreatingSwapchain;

    i32 (*FindMemoryIndex)(u32 _typeFilter, u32 _propertyFlags);
} VulkanContext;