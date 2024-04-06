#include "VulkanSwapchain.h"

#include "VulkanDevice.h"
#include "VulkanImage.h"

#include "core/Logger.h"
#include "core/CMemory.h"

void create(VulkanContext* _context, u32 _width, u32 _height, VulkanSwapchain* _swapchain);
void destroy(VulkanContext* _context, VulkanSwapchain* _swapchain);

void VulkanSwapchainCreate(
    VulkanContext* _context,
    u32 _width,
    u32 _height,
    VulkanSwapchain* _outSwapchain)
{
    //simply create new swapchain
    create(_context, _width, _height, _outSwapchain);
}

void VulkanSwapchainRecreate(
    VulkanContext* _context,
    u32 _width,
    u32 _height,
    VulkanSwapchain* _swapchain)
{
    //destroy the old create the new one
    destroy(_context, _swapchain);
    create(_context, _width, _height, _swapchain);
}

void VulkanSwapchainDestroy(VulkanContext* _context, VulkanSwapchain* _swapchain)
{
    destroy(_context, _swapchain);
}

b8 VulkanSwapchainAquireNextImageIndex(
    VulkanContext* _context,
    VulkanSwapchain* _swapchain,
    u64 _timeoutNS,
    VkSemaphore _imageAvailableSemaphore,
    VkFence _fence,
    u32* _outImageIndex)
{
    VkResult result = vkAcquireNextImageKHR(_context->device.logicalDevice, 
        _swapchain->handle, 
        _timeoutNS, 
        _imageAvailableSemaphore, 
        _fence, 
        _outImageIndex);
    
    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        //trigger swapchain recreation, then return from render loop
        VulkanSwapchainRecreate(_context, _context->framebufferWidth, _context->framebufferHeight, _swapchain);
        return FALSE;
    }
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        LOG_FATAL("Failed to acquire swapchain image.");
        return FALSE;
    }

    return TRUE;
}

void VulkanSwapchainPresent(
    VulkanContext* _context,
    VulkanSwapchain* _swapchain,
    VkQueue _graphicsQueue,
    VkQueue _presentQueue,
    VkSemaphore _renderCompleteSemaphore,
    u32 _presentImageIndex)
{
    //return the image to the swapchain for presentation
    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &_renderCompleteSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &_swapchain->handle;
    presentInfo.pImageIndices = &_presentImageIndex;
    presentInfo.pResults = 0;

    VkResult result = vkQueuePresentKHR(_presentQueue, &presentInfo);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        //swapchain is out of date, suboptimal or a framebuffer resize has occured, trigger swapchain recreation
        VulkanSwapchainRecreate(_context, _context->framebufferWidth, _context->framebufferHeight, _swapchain);
    }
    else if (result != VK_SUCCESS)
        LOG_FATAL("Failed to present swapchain image.");

    //increment (and loop) the index
    _context->currentFrame = (_context->currentFrame + 1) % _swapchain->maxFramesInFlight;
}

void create(VulkanContext* _context, u32 _width, u32 _height, VulkanSwapchain* _swapchain)
{
    LOG_INFO("Creating Vulkan swapchain...");

    VkExtent2D swapchainExtent = { _width, _height };
    _swapchain->maxFramesInFlight = 2; //triple buffering

    //choose swap surface format
    b8 found = FALSE;
    for(u32 i = 0; i < _context->device.swapchainSupport.formatCount; ++i)
    {
        VkSurfaceFormatKHR format = _context->device.swapchainSupport.formats[i];
        //prefered formats
        if(format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            _swapchain->imageFormat = format;
            found = TRUE;
            break;
        }
    }

    if(!found)
        _swapchain->imageFormat = _context->device.swapchainSupport.formats[0];

    //present mode
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for(u32 i = 0; i < _context->device.swapchainSupport.presentModeCount; ++i)
    {
        VkPresentModeKHR mode = _context->device.swapchainSupport.presentModes[i];
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = mode;
            break;
        }
    }

    //requery swapchain support
    VulkanDeviceQuerySwapchainSupport(_context->device.physicalDevice, _context->surface, &_context->device.swapchainSupport);

    //swapchain extent
    if(_context->device.swapchainSupport.capabilities.currentExtent.width != 0xFFFFFFFF)
        swapchainExtent = _context->device.swapchainSupport.capabilities.currentExtent;
    
    //clamp value allowed by the gpu
    VkExtent2D min = _context->device.swapchainSupport.capabilities.minImageExtent;
    VkExtent2D max = _context->device.swapchainSupport.capabilities.maxImageExtent;
    swapchainExtent.width = CCLAMP(swapchainExtent.width, min.width, max.width);
    swapchainExtent.height = CCLAMP(swapchainExtent.height, min.height, max.height);

    u32 imageCount = _context->device.swapchainSupport.capabilities.minImageCount + 1;
    if(_context->device.swapchainSupport.capabilities.maxImageCount > 0 && imageCount > _context->device.swapchainSupport.capabilities.maxImageCount)
        imageCount = _context->device.swapchainSupport.capabilities.maxImageCount;

    //swapchain create info
    VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapchainCreateInfo.surface = _context->surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = _swapchain->imageFormat.format;
    swapchainCreateInfo.imageColorSpace = _swapchain->imageFormat.colorSpace;
    swapchainCreateInfo.imageExtent = swapchainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    //setup queue family indices
    if(_context->device.graphicsQueueIndex != _context->device.presentQueueIndex)
    {
        u32 queueFamilyIndices[] = {
            (u32)_context->device.graphicsQueueIndex,
            (u32)_context->device.presentQueueIndex};
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = 0;
    }

    swapchainCreateInfo.preTransform = _context->device.swapchainSupport.capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = 0;

    VK_CHECK(vkCreateSwapchainKHR(_context->device.logicalDevice, &swapchainCreateInfo, _context->allocator, &_swapchain->handle));

    //start with a 0 frame index
    _context->currentFrame = 0;

    //images
    _swapchain->imageCount = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(_context->device.logicalDevice, _swapchain->handle, &_swapchain->imageCount, 0));
    if(!_swapchain->images)
        _swapchain->images = (VkImage*)cAllocate(sizeof(VkImage) * _swapchain->imageCount, MEMORY_TAG_RENDERER);
    if(!_swapchain->views)
        _swapchain->views = (VkImageView*)cAllocate(sizeof(VkImageView) * _swapchain->imageCount, MEMORY_TAG_RENDERER);
    VK_CHECK(vkGetSwapchainImagesKHR(_context->device.logicalDevice, _swapchain->handle, &_swapchain->imageCount, _swapchain->images));

    //views
    for(u32 i = 0; i < _swapchain->imageCount; ++i)
    {
        VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewInfo.image = _swapchain->images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = _swapchain->imageFormat.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(_context->device.logicalDevice, &viewInfo, _context->allocator, &_swapchain->views[i]));
    }

    //depth resources
    if(!VulkanDeviceDetectDepthFormat(&_context->device))
    {
        _context->device.depthFormat = VK_FORMAT_UNDEFINED;
        LOG_FATAL("Failed to find a supported format.");
    }

    VulkanImageCreate(
        _context,
        VK_IMAGE_TYPE_2D,
        swapchainExtent.width,
        swapchainExtent.height,
        _context->device.depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        TRUE,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &_swapchain->depthAttachment);

    LOG_INFO("Vulkan swapchain created successfully.");
}

void destroy(VulkanContext* _context, VulkanSwapchain* _swapchain)
{
    VulkanImageDestroy(_context, &_swapchain->depthAttachment);

    //only destroy the view, not the images, since those are owned by swapchain
    for(u32 i = 0; i < _swapchain->imageCount; ++i)
        vkDestroyImageView(_context->device.logicalDevice, _swapchain->views[i], _context->allocator);
    
    vkDestroySwapchainKHR(_context->device.logicalDevice, _swapchain->handle, _context->allocator);
}