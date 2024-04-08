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

    VkCommandPool graphicsCommandPool;

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

typedef struct VulkanFramebuffer
{
    VkFramebuffer handle;
    u32 attachmentCount;
    VkImageView* attachments;
    VulkanRenderpass* renderpass;
} VulkanFramebuffer;

typedef struct VulkanSwapchain
{
    VkSurfaceFormatKHR imageFormat;
    u8 maxFramesInFlight;
    VkSwapchainKHR handle;
    u32 imageCount;
    VkImage* images;
    VkImageView* views;

    VulkanImage depthAttachment;

    //framebuffers used for rendering to the screen.
    VulkanFramebuffer* framebuffers;
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

typedef struct VulkanFence
{
    VkFence handle;
    b8 isSignaled;
} VulkanFence;

typedef struct VulkanShaderStage
{
    VkShaderModuleCreateInfo createInfo;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo;
} VulkanShaderStage;

typedef struct VulkanPipeline
{
    VkPipeline handle;
    VkPipelineLayout pipelineLayout;
} VulkanPipeline;

#define OBJECT_SHADER_STAGE_COUNT 2

typedef struct VulkanObjectShader
{
    //vertex, fragment
    VulkanShaderStage stages[OBJECT_SHADER_STAGE_COUNT];

    VulkanPipeline pipeline;
} VulkanObjectShader;

typedef struct VulkanContext
{
    //the framebuffers current width and height
    u32 framebufferWidth;
    u32 framebufferHeight;

    //current generation of framebuffer size, if it does not match the last generation
    //a new one should be generated
    u64 framebufferSizeGeneration;

    //keeps track of the last time the framebuffer was created, set when updated
    u64 framebufferSizeLastGeneration;

    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
    
#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debugMessenger;
#endif

    VulkanDevice device;

    VulkanSwapchain swapchain;
    VulkanRenderpass mainRenderpass;

    //darray commandbuffers
    VulkanCommandBuffer* graphicsCommandBuffers;

    //darray image semaphore
    VkSemaphore* imageAvailableSemaphores;

    //darray queue semaphores
    VkSemaphore* queueCompleteSemaphores;

    u32 inFlightFenceCount;
    VulkanFence* inFlightFences;

    //holds pointers to fences that exist and owned somewhere else
    VulkanFence** imagesInFlight;

    u32 imageIndex;
    u32 currentFrame;

    b8 recreatingSwapchain;

    VulkanObjectShader objectShader;

    i32 (*FindMemoryIndex)(u32 _typeFilter, u32 _propertyFlags);
} VulkanContext;