#include "VulkanFence.h"

#include "core/Logger.h"

void VulkanFenceCreate(VulkanContext* _context, b8 _createSignaled, VulkanFence* _outFence) 
{
    //make sure to signal fence if required
    _outFence->isSignaled = _createSignaled;

    VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    if(_outFence->isSignaled)
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateFence(_context->device.logicalDevice, &fenceCreateInfo, _context->allocator, &_outFence->handle));
}

void VulkanFenceDestroy(VulkanContext* _context, VulkanFence* _fence) 
{
    if(_fence->handle)
        vkDestroyFence(_context->device.logicalDevice, _fence->handle, _context->allocator);
    
    _fence->isSignaled = FALSE;
}

b8 VulkanFenceWait(VulkanContext* _context, VulkanFence* _fence, u64 _timeoutNS) 
{
    if(!_fence->isSignaled)
    {
        VkResult result = vkWaitForFences(_context->device.logicalDevice, 1, &_fence->handle, TRUE, _timeoutNS);

        switch(result)
        {
            case VK_SUCCESS:
                _fence->isSignaled = TRUE;
                return TRUE;
            case VK_TIMEOUT:
                LOG_WARN("vk_fence_wait - Timed out");
                break;
            case VK_ERROR_DEVICE_LOST:
                LOG_ERROR("vk_fence_wait - VK_ERROR_DEVICE_LOST");
                break;
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                LOG_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_HOST_MEMORY");
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                LOG_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_DEVICE_MEMORY");
                break;
            default:
                LOG_ERROR("vk_fence_wait - Unknown error has occured.");
                break;
        }
    }
    else
    {
        //if already signaled do not wait
        return TRUE;
    }

    return FALSE;
}

void VulkanFenceReset(VulkanContext* _context, VulkanFence* _fence) 
{
    if(_fence->isSignaled)
    {
        VK_CHECK(vkResetFences(_context->device.logicalDevice,1, &_fence->handle));
        _fence->isSignaled = FALSE;
    }
}