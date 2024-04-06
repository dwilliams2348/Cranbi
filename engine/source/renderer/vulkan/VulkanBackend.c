#include "VulkanBackend.h"

#include "VulkanTypes.inl"
#include "VulkanPlatform.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderpass.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanFence.h"

#include "core/Logger.h"
#include "core/CString.h"
#include "core/CMemory.h"
#include "core/Application.h"

#include "containers/DArray.h"


//static vulkan context
static VulkanContext context;
static u32 cachedFramebufferWidth = 0;
static u32 cachedFramebufferHeight = 0;

VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);

i32 FindMemoryIndex(u32 _typeFilter, u32 _propertyFlags);

void CreateCommandBuffers(RendererBackend* _backend);
void RegenerateFramebuffers(RendererBackend* _backend, VulkanSwapchain* _swapchain, VulkanRenderpass* _renderpass);

b8 VulkanRendererBackendInitialize(RendererBackend* _backend, const char* _appName, struct PlatformState* _platform)
{
    //function pointers
    context.FindMemoryIndex = FindMemoryIndex;

    //TODO: custom allocator
    context.allocator = 0;

    ApplicationGetFramebufferSize(&cachedFramebufferWidth, &cachedFramebufferHeight);
    context.framebufferWidth = (cachedFramebufferWidth != 0) ? cachedFramebufferWidth : 800;
    context.framebufferHeight = (cachedFramebufferHeight != 0) ? cachedFramebufferHeight : 600;
    cachedFramebufferWidth = 0;
    cachedFramebufferHeight = 0;

    //setup vulkan instance
    VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.pApplicationName = _appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Cranbi Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    createInfo.pApplicationInfo = &appInfo;

    //obtain list of required extensions
    const char** requiredExtensions = DArrayCreate(const char*);
    DArrayPush(requiredExtensions, &VK_KHR_SURFACE_EXTENSION_NAME); //generic surface extension
    PlatformGetRequiredExtensionNames(&requiredExtensions);         //get platform specific extension(s)
#if defined(_DEBUG)
    DArrayPush(requiredExtensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME); //debug utilities

    LOG_DEBUG("Required extensions:");
    u32 length = DArrayLength(requiredExtensions);
    for(u32 i = 0; i < length; ++i)
    {
        LOG_DEBUG(requiredExtensions[i]);
    }
#endif

    createInfo.enabledExtensionCount = DArrayLength(requiredExtensions);
    createInfo.ppEnabledExtensionNames = requiredExtensions;

    //validation layers
    const char** requiredValidationLayerNames = 0;
    u32 requiredValidationLayerCount = 0;

//if validation should be done, get a list of the required validation layer names
//and make sure they exist, Validation layers should only be enabled on non-release
#if defined(_DEBUG)
    LOG_INFO("Validation layers enabled, Enumerating...");

    //the list of validation layers required
    requiredValidationLayerNames = DArrayCreate(const char*);
    DArrayPush(requiredValidationLayerNames, &"VK_LAYER_KHRONOS_validation");
    requiredValidationLayerCount = DArrayLength(requiredValidationLayerNames);

    //obtain a list of availiable validation layers
    u32 availableLayerCount = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, 0));
    VkLayerProperties* availableLayers = DArrayReserve(VkLayerProperties, availableLayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers));

    //verify all required layers are available
    for(u32 i = 0; i < requiredValidationLayerCount; ++i)
    {
        LOG_INFO("Searching for layer: %s...", requiredValidationLayerNames[i])
    
        b8 found = FALSE;
        for(u32 j = 0; j < availableLayerCount; ++j)
        {
            if(StringsEqual(requiredValidationLayerNames[i], availableLayers[j].layerName))
            {
                found = TRUE;
                LOG_INFO("Found.");
                break;
            }
        }

        if(!found)
        {
            LOG_FATAL("Required validation layer is missing: %s", requiredValidationLayerNames[i]);
            return FALSE;
        }
    }

    LOG_INFO("All required validation layers are present.");
#endif

    createInfo.enabledLayerCount = requiredValidationLayerCount;
    createInfo.ppEnabledLayerNames = requiredValidationLayerNames;

    VK_CHECK(vkCreateInstance(&createInfo, context.allocator, &context.instance));
    LOG_INFO("Vulkan instance created.");

//debugger
#if defined(_DEBUG)
    LOG_DEBUG("Creating Vulkan debugger...");
    u32 logSeverity =   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT; //|
                        //VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;  //|
                        //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    debugCreateInfo.messageSeverity = logSeverity;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugCreateInfo.pfnUserCallback = VKDebugCallback;

    PFN_vkCreateDebugUtilsMessengerEXT func = 
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
    CORE_ASSERT_MSG(func, "Failed to create debug messenger.");
    VK_CHECK(func(context.instance, &debugCreateInfo, context.allocator, &context.debugMessenger));
    LOG_DEBUG("Vulkan debugger created.");
#endif

    //surface creation
    LOG_INFO("Creating Vulkan surface...");
    if(!PlatformCreateVulkanSurface(_platform, &context))
    {
        LOG_ERROR("Failed to create platform surface.");
        return FALSE;
    }
    LOG_INFO("Vulkan surface created");

    //device creation
    if(!VulkanDeviceCreate(&context))
    {
        LOG_ERROR("Failed to create Vulkan device.");
        return FALSE;
    }

    //swapchain creation
    LOG_INFO("Creating Vulkan swapchain...");
    VulkanSwapchainCreate(&context, context.framebufferWidth, context.framebufferHeight, &context.swapchain);

    //renderpass creation
    LOG_INFO("Creating Vulkan renderpass...");
    VulkanRenderpassCreate(&context, &context.mainRenderpass, 
    0, 0, context.framebufferWidth, context.framebufferHeight,
    0.f, 0.f, 0.2f, 1.f,
    1.f, 0);

    //create swapchain framebuffers
    LOG_INFO("Creating Vulkan swapchain framebuffers...");
    context.swapchain.framebuffers = DArrayReserve(VulkanFramebuffer, context.swapchain.imageCount);
    RegenerateFramebuffers(_backend, &context.swapchain, &context.mainRenderpass);

    //create command buffers
    LOG_INFO("Creating Vulkan command buffers...");
    CreateCommandBuffers(_backend);

    //create sync objects
    LOG_INFO("Creating Vulkan sync objects...");
    context.imageAvailableSemaphores = DArrayReserve(VkSemaphore, context.swapchain.maxFramesInFlight);
    context.queueCompleteSemaphores = DArrayReserve(VkSemaphore, context.swapchain.maxFramesInFlight);
    context.inFlightFences = DArrayReserve(VulkanFence, context.swapchain.maxFramesInFlight);

    for(u8 i = 0; i < context.swapchain.maxFramesInFlight; ++i)
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        vkCreateSemaphore(context.device.logicalDevice, &semaphoreCreateInfo, context.allocator, &context.imageAvailableSemaphores[i]);
        vkCreateSemaphore(context.device.logicalDevice, &semaphoreCreateInfo, context.allocator, &context.queueCompleteSemaphores[i]);

        //create the fence in a signaled state, indicating that the first frame has already been "rendered"
        //this will prevent the application from waiting indefinitely for the first frame to render since it cannot render until the previous one is rendered.
        VulkanFenceCreate(&context, TRUE, &context.inFlightFences[i]);
    }

    //in lfight fences should not exist at this time, clear the list, These are stored in pointers
    //because the intial state should be 0, will be 0 when not in use, actual fences are not owned by the list
    context.imagesInFlight = DArrayReserve(VulkanFence, context.swapchain.imageCount);
    for(u32 i = 0; i < context.swapchain.imageCount; ++i)
    {
        context.imagesInFlight[i] = 0;
    }

    LOG_INFO("Vulkan renderer initialized successfully");
    return TRUE;
}

void VulkanRendererBackendShutdown(RendererBackend* _backend)
{
    vkDeviceWaitIdle(context.device.logicalDevice);

    //destroy in opposite order of creation

    //sync objects
    LOG_DEBUG("Destroying Vulkan sync objects...");
    for(u8 i = 0; i < context.swapchain.maxFramesInFlight; ++i)
    {
        if(context.imageAvailableSemaphores[i])
        {
            vkDestroySemaphore(context.device.logicalDevice, context.imageAvailableSemaphores[i], context.allocator);
            context.imageAvailableSemaphores[i] = 0;
        }
        if(context.queueCompleteSemaphores[i])
        {
            vkDestroySemaphore(context.device.logicalDevice, context.queueCompleteSemaphores[i], context.allocator);
            context.queueCompleteSemaphores[i] = 0;
        }

        VulkanFenceDestroy(&context, &context.inFlightFences[i]);
    }

    DArrayDestroy(context.imageAvailableSemaphores);
    context.imageAvailableSemaphores = 0;

    DArrayDestroy(context.queueCompleteSemaphores);
    context.queueCompleteSemaphores = 0;

    DArrayDestroy(context.inFlightFences);
    context.inFlightFences = 0;

    DArrayDestroy(context.imagesInFlight);
    context.imagesInFlight = 0;

    //command buffers
    LOG_DEBUG("Destroying Vulkan command buffers...");
    for(u32 i = 0; i < context.swapchain.imageCount; ++i)
    {
        if(context.graphicsCommandBuffers[i].handle)
        {
            VulkanCommandBufferFree(&context, context.device.graphicsCommandPool, &context.graphicsCommandBuffers[i]);
            context.graphicsCommandBuffers[i].handle = 0;
        }
    }

    DArrayDestroy(context.graphicsCommandBuffers);
    context.graphicsCommandBuffers = 0;

    //destroy framebuffers
    LOG_DEBUG("Destroying Vulkan framebuffers...");
    for(u32 i = 0; i < context.swapchain.imageCount; ++i)
        VulkanFramebufferDestroy(&context, &context.swapchain.framebuffers[i]);

    //renderpass
    LOG_DEBUG("Destroying Vulkan renderpass...");
    VulkanRenderpassDestroy(&context, &context.mainRenderpass);

    //swapchain
    LOG_DEBUG("Destroying Vulkan swapchain...");
    VulkanSwapchainDestroy(&context, &context.swapchain);

    LOG_DEBUG("Destroying Vulkan device...");
    VulkanDeviceDestroy(&context);

    LOG_DEBUG("Destroying Vulkan surface...");
    if(context.surface)
    {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = 0;
    }

    LOG_DEBUG("Destroying Vulkan debugger...");
    if (context.debugMessenger) 
    {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(context.instance, context.debugMessenger, context.allocator);
    }

    LOG_DEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(context.instance, context.allocator);
}

void VulkanRendererBackendOnResize(RendererBackend* _backend, u16 _width, u16 _height)
{

}

b8 VulkanRendererBackendBeginFrame(RendererBackend* _backend, f32 _deltaTime)
{
    return TRUE;
}

b8 VulkanRendererBackendEndFrame(RendererBackend* _backend, f32 _deltaTime)
{
    return TRUE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) 
    {
    switch (message_severity) 
    {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_TRACE(callback_data->pMessage);
            break;
    }

    return VK_FALSE;
}

i32 FindMemoryIndex(u32 _typeFilter, u32 _propertyFlags)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(context.device.physicalDevice, &memoryProperties);

    for(u32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        //check each memory type to see if its bit is set to 1
        if(_typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & _propertyFlags) == _propertyFlags)
            return 1;
    }

    LOG_WARN("Unable to find suitable memory type.");
    return -1;
}

void CreateCommandBuffers(RendererBackend* _backend)
{
    if(!context.graphicsCommandBuffers)
    {
        context.graphicsCommandBuffers = DArrayReserve(VulkanCommandBuffer, context.swapchain.imageCount);
        for(u32 i = 0; i < context.swapchain.imageCount; ++i)
            cZeroMemory(&context.graphicsCommandBuffers[i], sizeof(VulkanCommandBuffer));
    }

    for(u32 i = 0; i < context.swapchain.imageCount; ++i)
    {
        if(context.graphicsCommandBuffers[i].handle)
            VulkanCommandBufferFree(&context, context.device.graphicsCommandPool, &context.graphicsCommandBuffers[i]);

        cZeroMemory(&context.graphicsCommandBuffers[i], sizeof(VulkanCommandBuffer));
        VulkanCommandBufferAllocate(&context, context.device.graphicsCommandPool, TRUE, &context.graphicsCommandBuffers[i]);
    }

    LOG_INFO("Vulkan command buffers created.");
}

void RegenerateFramebuffers(RendererBackend* _backend, VulkanSwapchain* _swapchain, VulkanRenderpass* _renderpass)
{
    for(u32 i = 0; i < _swapchain->imageCount; ++i)
    {
        //TODO: make this dynamic based on the currently configured attachments
        u32 attachmentCount = 2;
        VkImageView attachments[] = { _swapchain->views[i], _swapchain->depthAttachment.view };

        VulkanFramebufferCreate(&context,
            _renderpass,
            context.framebufferWidth, context.framebufferHeight,
            attachmentCount,
            attachments,
            &context.swapchain.framebuffers[i]);
    }

    LOG_INFO("Vulkan framebuffers created.");
}