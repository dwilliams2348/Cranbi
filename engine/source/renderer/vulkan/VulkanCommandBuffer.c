#include "VulkanCommandBuffer.h"

#include "core/CMemory.h"

void VulkanCommandBufferAllocate(VulkanContext* _context, VkCommandPool _pool, b8 _isPrimary, VulkanCommandBuffer* _outCommandBuffer) 
{
    cZeroMemory(_outCommandBuffer, sizeof(_outCommandBuffer));

    VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocateInfo.commandPool = _pool;
    allocateInfo.level = _isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.pNext = 0;

    _outCommandBuffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
    VK_CHECK(vkAllocateCommandBuffers(_context->device.logicalDevice, &allocateInfo, &_outCommandBuffer->handle));
    _outCommandBuffer->state = COMMAND_BUFFER_STATE_READY;
}

void VulkanCommandBufferFree(VulkanContext* _context, VkCommandPool _pool, VulkanCommandBuffer* _commandBuffer) 
{
    vkFreeCommandBuffers(_context->device.logicalDevice, _pool, 1, &_commandBuffer->handle);
    _commandBuffer->handle = 0;
    _commandBuffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

void VulkanCommandBufferBegin(VulkanCommandBuffer* _commandBuffer, b8 _isSingleUse, b8 _isRenderpassContinue, b8 _isSimultaneousUse) 
{
    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = 0;
    if(_isSingleUse)
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if(_isRenderpassContinue)
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    if(_isSimultaneousUse)
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VK_CHECK(vkBeginCommandBuffer(_commandBuffer->handle, &beginInfo));
    _commandBuffer->state = COMMAND_BUFFER_STATE_RECORDING;
}

void VulkanCommandBufferEnd(VulkanCommandBuffer* _commandBuffer) 
{
    VK_CHECK(vkEndCommandBuffer(_commandBuffer->handle));
    _commandBuffer->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
}

void VulkanCommandBufferUpdateSubmitted(VulkanCommandBuffer* _commandBuffer) 
{
    _commandBuffer->state = COMMAND_BUFFER_STATE_SUBMITTED;
}

void VulkanCommandBufferReset(VulkanCommandBuffer* _commandBuffer) 
{
    _commandBuffer->state = COMMAND_BUFFER_STATE_READY;
}

void VulkanCommandBufferAllocateAndBeginSingleUse(VulkanContext* _context, VkCommandPool _pool, VulkanCommandBuffer* _outCommandBuffer) 
{
    VulkanCommandBufferAllocate(_context, _pool, TRUE, _outCommandBuffer);
    VulkanCommandBufferBegin(_outCommandBuffer, TRUE, FALSE, FALSE);
}

void VulkanCommandBufferEndSingleUse(VulkanContext* _context, VkCommandPool _pool, VulkanCommandBuffer* _commandBuffer, VkQueue _queue) 
{
    //end the command buffer
    VulkanCommandBufferEnd(_commandBuffer);

    //submits the queue
    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer->handle;
    VK_CHECK(vkQueueSubmit(_queue, 1, &submitInfo, 0));

    //wait for it to finish
    VK_CHECK(vkQueueWaitIdle(_queue));

    //free the command buffer
    VulkanCommandBufferFree(_context, _pool, _commandBuffer);
}