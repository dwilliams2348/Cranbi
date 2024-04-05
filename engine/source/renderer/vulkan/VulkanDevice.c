#include "VulkanDevice.h"
#include "core/Logger.h"
#include "core/CString.h"
#include "core/CMemory.h"
#include "containers/DArray.h"

typedef struct VulkanPhysicalDeviceRequirements
{
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;
    //darray
    const char** deviceExtensionNames;
    b8 samplerAnisotropy;
    b8 discreteGPU;
} VulkanPhysicalDeviceRequirements;

typedef struct VulkanPhysicalDeviceQueueFamilyInfo
{
    u32 graphicsFamilyIndex;
    u32 presentFamilyIndex;
    u32 computeFamilyIndex;
    u32 transferFamilyIndex;
} VulkanPhysicalDeviceQueueFamilyInfo;

b8 SelectPhysicalDevice(VulkanContext* _context);
b8 PhysicalDeviceMeetsRequirements(
    VkPhysicalDevice _device,
    VkSurfaceKHR _surface,
    const VkPhysicalDeviceProperties* _props,
    const VkPhysicalDeviceFeatures* _features,
    const VulkanPhysicalDeviceRequirements* _requirements,
    VulkanPhysicalDeviceQueueFamilyInfo* _outQueueInfo,
    VulkanSwapchainSupportInfo* _outSwapchainSupport);

b8 VulkanDeviceCreate(VulkanContext* _context)
{
    if(!SelectPhysicalDevice(_context))
        return FALSE;

    return TRUE;
}

void VulkanDeviceDestroy(VulkanContext* _context)
{

}

void VulkanDeviceQuerySwapchainSupport(
    VkPhysicalDevice _physicalDevice,
    VkSurfaceKHR _surface,
    VulkanSwapchainSupportInfo* _outSupportInfo)
{
    //surface capabilities
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice,_surface, &_outSupportInfo->capabilities));

    //surface formats
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &_outSupportInfo->formatCount, 0));
    if(_outSupportInfo->formatCount != 0)
    {
        if(!_outSupportInfo->formats)
            _outSupportInfo->formats = cAllocate(sizeof(VkSurfaceFormatKHR) * _outSupportInfo->formatCount, MEMORY_TAG_RENDERER);

        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &_outSupportInfo->formatCount, _outSupportInfo->formats));
    }

    //present modes
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &_outSupportInfo->presentModeCount, 0));
    if(_outSupportInfo->presentModeCount != 0)
    {
        if(!_outSupportInfo->presentModes)
            _outSupportInfo->presentModes = cAllocate(sizeof(VkPresentModeKHR) * _outSupportInfo->presentModeCount, MEMORY_TAG_RENDERER);

        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &_outSupportInfo->presentModeCount, _outSupportInfo->presentModes));
    }
}

b8 SelectPhysicalDevice(VulkanContext* _context)
{
    u32 physicalDeviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(_context->instance, &physicalDeviceCount, 0));
    if(physicalDeviceCount == 0)
    {
        LOG_FATAL("No devices which support Vulkan were found.");
        return FALSE;
    }

    VkPhysicalDevice physicalDevices[physicalDeviceCount];
    VK_CHECK(vkEnumeratePhysicalDevices(_context->instance, &physicalDeviceCount, physicalDevices));
    for(u32 i = 0; i < physicalDeviceCount; ++i)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &memory);

        //TODO: requirements should be driven by engine configuration
        VulkanPhysicalDeviceRequirements requirements = {};
        requirements.graphics = TRUE;
        requirements.present = TRUE;
        requirements.transfer = TRUE;
        //NOTE: enable this is compute will be required
        //requirements.compute = TRUE;
        requirements.samplerAnisotropy = TRUE;
        requirements.discreteGPU = TRUE;
        requirements.deviceExtensionNames = DArrayCreate(const char*);
        DArrayPush(requirements.deviceExtensionNames, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        VulkanPhysicalDeviceQueueFamilyInfo queueInfo = {};
        b8 result = PhysicalDeviceMeetsRequirements(
            physicalDevices[i],
            _context->surface,
            &properties,
            &features,
            &requirements,
            &queueInfo,
            &_context->device.swapchainSupport);

        if(result)
        {
            LOG_INFO("Selected device: '%s'.", properties.deviceName);
            // GPU type, etc.
            switch (properties.deviceType) 
            {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    LOG_INFO("GPU type is Unknown.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    LOG_INFO("GPU type is Integrated.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    LOG_INFO("GPU type is Descrete.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    LOG_INFO("GPU type is Virtual.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    LOG_INFO("GPU type is CPU.");
                    break;
            }

            //driver info
            LOG_INFO(
                "GPU Driver version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.driverVersion),
                VK_VERSION_MINOR(properties.driverVersion),
                VK_VERSION_PATCH(properties.driverVersion));

            //vulkan API version
            LOG_INFO(
                "Vulkan API version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.apiVersion),
                VK_VERSION_MINOR(properties.apiVersion),
                VK_VERSION_PATCH(properties.apiVersion));

            //memory information
            for(u32 j = 0; j < memory.memoryHeapCount; ++j)
            {
                f32 memorySizeGiB = (((f32)memory.memoryHeaps[j].size) / 1024.0f / 1024.0f / 1024.0f);
                if(memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                {
                    LOG_INFO("Local GPU memory: %.2f GiB", memorySizeGiB);
                }
                else
                {
                    LOG_INFO("Shared System memory: %.2f GiB", memorySizeGiB);
                }
            }

            _context->device.physicalDevice = physicalDevices[i];
            _context->device.graphicsQueueIndex = queueInfo.graphicsFamilyIndex;
            _context->device.presentQueueIndex = queueInfo.presentFamilyIndex;
            _context->device.transferQueueIndex = queueInfo.transferFamilyIndex;
            //NOTE: set compute index here if needed.

            //keep a copy of properties, features and memory for later use
            _context->device.properties = properties;
            _context->device.features = features;
            _context->device.memory = memory;

            break;
        }
    }

    //ensure a device was selected
    if(!_context->device.physicalDevice)
    {
        LOG_ERROR("No physical devices were found which meet the requirements.");
        return FALSE;
    }

    LOG_INFO("Physical device was selected.");
    return TRUE;
}

b8 PhysicalDeviceMeetsRequirements(
    VkPhysicalDevice _device,
    VkSurfaceKHR _surface,
    const VkPhysicalDeviceProperties* _props,
    const VkPhysicalDeviceFeatures* _features,
    const VulkanPhysicalDeviceRequirements* _requirements,
    VulkanPhysicalDeviceQueueFamilyInfo* _outQueueInfo,
    VulkanSwapchainSupportInfo* _outSwapchainSupport)
{
    //evaluate device properties to determine if it meets the need of the application
    _outQueueInfo->graphicsFamilyIndex = -1;
    _outQueueInfo->presentFamilyIndex = -1;
    _outQueueInfo->computeFamilyIndex = -1;
    _outQueueInfo->transferFamilyIndex = -1;

    //discrete gpu?
    if(_requirements->discreteGPU)
    {
        if(_props->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            LOG_INFO("Device is not a discrete GPU, and one is required skipping.");
            return FALSE;
        }
    }

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, 0);
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies);

    //look at each queue and see what it supports
    LOG_INFO("Graphics | Present | Compute | Transfer | Name");
    u8 minTransferScore = 255;
    for(u32 i = 0; i < queueFamilyCount; ++i)
    {
        u8 currentTransferScore = 0;

        //graphics queue?
        if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            _outQueueInfo->graphicsFamilyIndex = i;
            ++currentTransferScore;
        }

        //compute queue?
        if(queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            _outQueueInfo->computeFamilyIndex = i;
            ++currentTransferScore;
        }

        //transfer queue?
        if(queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            //take the index if it is the current lowest, this increases the likihood that it is a dedicated transfer queue
            if(currentTransferScore <= minTransferScore)
            {
                minTransferScore = currentTransferScore;
                _outQueueInfo->transferFamilyIndex = i;
            }
        }

        //present queue?
        VkBool32 supportsPresent = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, _surface, &supportsPresent));
        if(supportsPresent)
        {
            _outQueueInfo->presentFamilyIndex = i;
        }
    }

    LOG_INFO("       %d |       %d |       %d |        %d | %s",
            _outQueueInfo->graphicsFamilyIndex != -1,
            _outQueueInfo->presentFamilyIndex != 1,
            _outQueueInfo->computeFamilyIndex != 1,
            _outQueueInfo->transferFamilyIndex != -1,
            _props->deviceName);

    if(
        (!_requirements->graphics || (_requirements->graphics && _outQueueInfo->graphicsFamilyIndex != -1)) &&
        (!_requirements->present || (_requirements->present && _outQueueInfo->presentFamilyIndex != -1)) &&
        (!_requirements->compute || (_requirements->compute && _outQueueInfo->computeFamilyIndex != -1)) &&
        (!_requirements->transfer || (_requirements->transfer && _outQueueInfo->transferFamilyIndex != -1)))
    {
        LOG_INFO("Device meets queue requirements.");
        LOG_TRACE("Graphics Family Index: %i", _outQueueInfo->graphicsFamilyIndex);
        LOG_TRACE("Present Family Index:  %i", _outQueueInfo->presentFamilyIndex);
        LOG_TRACE("Transfer Family Index: %i", _outQueueInfo->transferFamilyIndex);
        LOG_TRACE("Compute Family Index:  %i", _outQueueInfo->computeFamilyIndex);

        //query swapchain support
        VulkanDeviceQuerySwapchainSupport(_device, _surface, _outSwapchainSupport);

        if(_outSwapchainSupport->formatCount < 1 || _outSwapchainSupport->presentModeCount < 1)
        {
            if(_outSwapchainSupport->formats)
            {
                cFree(_outSwapchainSupport->formats, sizeof(VkSurfaceFormatKHR) * _outSwapchainSupport->formatCount, MEMORY_TAG_RENDERER);
            }
            if(_outSwapchainSupport->presentModes)
            {
                cFree(_outSwapchainSupport->presentModes, sizeof(VkPresentModeKHR) * _outSwapchainSupport->presentModeCount, MEMORY_TAG_RENDERER);
            }

            LOG_INFO("Required swapchain support not present skipping device.");
            return FALSE;
        }

        //device extensions
        if(_requirements->deviceExtensionNames)
        {
            u32 availableExtensionCount = 0;
            VkExtensionProperties* availableExtensions = 0;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(
                _device, 0, &availableExtensionCount, 0));
            if(availableExtensionCount != 0)
            {
                availableExtensions = cAllocate(sizeof(VkExtensionProperties) * availableExtensionCount, MEMORY_TAG_RENDERER);
                VK_CHECK(vkEnumerateDeviceExtensionProperties(
                _device, 0, &availableExtensionCount, availableExtensions));

                u32 requiredExtensionCount = DArrayLength(_requirements->deviceExtensionNames);
                for(u32 i = 0; i < requiredExtensionCount; ++i)
                {
                    b8 found = FALSE;
                    for(u32 j = 0; j < availableExtensionCount; ++j)
                    {
                        if(StringsEqual(_requirements->deviceExtensionNames[i], availableExtensions[j].extensionName))
                        {
                            found = TRUE;
                            break;
                        }
                    }

                    if(!found)
                    {
                        LOG_INFO("Required extension not found: %s, skipping device.", _requirements->deviceExtensionNames[i]);
                        cFree(availableExtensions, sizeof(VkExtensionProperties) * availableExtensionCount, MEMORY_TAG_RENDERER);
                        return FALSE;
                    }
                }
            }

            cFree(availableExtensions, sizeof(VkExtensionProperties) * availableExtensionCount, MEMORY_TAG_RENDERER);
        }

        //sampler anisotropy
        if(_requirements->samplerAnisotropy && !_features->samplerAnisotropy)
        {
            LOG_INFO("Device does not support samplerAnisotropy, skipping device.");
            return FALSE;
        }

        //device meets all requirements
        return TRUE;
    }

    return FALSE;
}