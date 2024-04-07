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

    LOG_INFO("Creating logical device...");

    //NOTE: Do not create additional queues for shared indices.
    b8 presentSharesGraphicsQueue = _context->device.graphicsQueueIndex == _context->device.presentQueueIndex;
    b8 transferSharesGraphicsQueue = _context->device.graphicsQueueIndex == _context->device.transferQueueIndex;
    u32 indexCount = 1;
    if(!presentSharesGraphicsQueue)
        indexCount++;

    if(!transferSharesGraphicsQueue)
        indexCount++;
    
    u32 indices[indexCount];
    u8 index = 0;
    indices[index++] = _context->device.graphicsQueueIndex;
    if(!presentSharesGraphicsQueue)
        indices[index++] = _context->device.presentQueueIndex;
    
    if(!transferSharesGraphicsQueue)
        indices[index++] = _context->device.transferQueueIndex;

    VkDeviceQueueCreateInfo queueCreateInfos[indexCount];
    for (u32 i = 0; i < indexCount; ++i) 
    {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].queueFamilyIndex = indices[i];
        queueCreateInfos[i].queueCount = 1;
        //TODO:Enable this for multithreading in future
        //if (indices[i] == _context->device.graphicsQueueIndex) 
        //{
        //    queueCreateInfos[i].queueCount = 2;
        //}
        queueCreateInfos[i].flags = 0;
        queueCreateInfos[i].pNext = 0;
        f32 queuePriority = 1.0f;
        queueCreateInfos[i].pQueuePriorities = &queuePriority;
    }

    //request device features
    //TODO: this should be config driven
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE; //request anistrophy

    VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    deviceCreateInfo.queueCreateInfoCount = indexCount;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 1;
    const char* extensionNames = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    deviceCreateInfo.ppEnabledExtensionNames = &extensionNames;

    //deprecated and ignored so pass nothing
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = 0;

    //Create the device
    VK_CHECK(vkCreateDevice(
        _context->device.physicalDevice,
        &deviceCreateInfo,
        _context->allocator,
        &_context->device.logicalDevice));

    LOG_INFO("Logical device created.");

    //get queues
    LOG_INFO("Getting queues...");
    vkGetDeviceQueue(
        _context->device.logicalDevice,
        _context->device.graphicsQueueIndex,
        0,
        &_context->device.graphicsQueue);

    vkGetDeviceQueue(
        _context->device.logicalDevice,
        _context->device.presentQueueIndex,
        0,
        &_context->device.presentQueue);

    vkGetDeviceQueue(
        _context->device.logicalDevice,
        _context->device.transferQueueIndex,
        0,
        &_context->device.transferQueue);
    LOG_INFO("Queues obtained.");

    //create command pool for graphics queue
    VkCommandPoolCreateInfo poolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    poolCreateInfo.queueFamilyIndex = _context->device.graphicsQueueIndex;
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CHECK(vkCreateCommandPool(_context->device.logicalDevice, &poolCreateInfo, _context->allocator, &_context->device.graphicsCommandPool));
    LOG_INFO("Graphics command pool created.");

    return TRUE;
}

void VulkanDeviceDestroy(VulkanContext* _context)
{
    //unset queues
    _context->device.graphicsQueue = 0;
    _context->device.presentQueue = 0;
    _context->device.transferQueue = 0;

    LOG_INFO("Destroying command pools...");
    vkDestroyCommandPool(_context->device.logicalDevice, _context->device.graphicsCommandPool, _context->allocator);

    //destroy logical device
    LOG_INFO("Destroying logical device...");
    if(_context->device.logicalDevice)
    {
        vkDestroyDevice(_context->device.logicalDevice, _context->allocator);
        _context->device.logicalDevice = 0;
    }

    //physical devices are not destroyed.
    LOG_INFO("Releasing physical device resources...");
    _context->device.physicalDevice = 0;

    if(_context->device.swapchainSupport.formats)
    {
        cFree(_context->device.swapchainSupport.formats,
        sizeof(VkSurfaceFormatKHR) * _context->device.swapchainSupport.formatCount,
        MEMORY_TAG_RENDERER);

        _context->device.swapchainSupport.formats = 0;
        _context->device.swapchainSupport.formatCount = 0;
    }

    if(_context->device.swapchainSupport.presentModes)
    {
        cFree(_context->device.swapchainSupport.presentModes,
        sizeof(VkPresentModeKHR) * _context->device.swapchainSupport.presentModeCount,
        MEMORY_TAG_RENDERER);

        _context->device.swapchainSupport.presentModes = 0;
        _context->device.swapchainSupport.presentModeCount = 0;
    }

    cZeroMemory(&_context->device.swapchainSupport.capabilities, sizeof(_context->device.swapchainSupport.capabilities));

    _context->device.graphicsQueueIndex = -1;
    _context->device.presentQueueIndex = -1;
    _context->device.transferQueueIndex = -1;
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

b8 VulkanDeviceDetectDepthFormat(VulkanDevice* _device)
{
    //format condidates
    const u64 candidateCount = 3;
    VkFormat candidates[3] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};

    u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    for(u64 i = 0; i < candidateCount; ++i)
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(_device->physicalDevice, candidates[i], &properties);

        if((properties.linearTilingFeatures & flags) == flags)
        {
            _device->depthFormat = candidates[i];
            return TRUE;
        }
        else if((properties.optimalTilingFeatures & flags) == flags)
        {
            _device->depthFormat = candidates[i];
            return TRUE;
        }
    }

    return FALSE;
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
                    LOG_INFO("GPU type is Discrete.");
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