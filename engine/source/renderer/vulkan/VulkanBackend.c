#include "VulkanBackend.h"

#include "VulkanTypes.inl"

#include "core/Logger.h"

//static vulkan context
static VulkanContext context;

b8 VulkanRendererBackendInitialize(RendererBackend* _backend, const char* _appName, struct PlatformState* _platform)
{
    //TODO: custom allocator
    context.allocator = 0;

    //setup vulkan instance
    VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.pApplicationName = _appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Cranbi Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = 0;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = 0;

    VkResult result = vkCreateInstance(&createInfo, context.allocator, &context.instance);
    if(result != VK_SUCCESS)
    {
        LOG_ERROR("VkCreateInstance failed with result: %u", result);
        return FALSE;
    }

    LOG_INFO("Vulkan renderer initialized successfully");
    return TRUE;
}

void VulkanRendererBackendShutdown(RendererBackend* _backend)
{

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