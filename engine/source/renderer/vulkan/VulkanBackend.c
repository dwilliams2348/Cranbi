#include "VulkanBackend.h"

#include "VulkanTypes.inl"
#include "VulkanPlatform.h"

#include "core/Logger.h"
#include "core/CString.h"

#include "containers/DArray.h"


//static vulkan context
static VulkanContext context;

VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);

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

    LOG_INFO("Vulkan renderer initialized successfully");
    return TRUE;
}

void VulkanRendererBackendShutdown(RendererBackend* _backend)
{
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