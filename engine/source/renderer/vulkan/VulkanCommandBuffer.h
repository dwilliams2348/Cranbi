#pragma once

#include "VulkanTypes.inl"

void VulkanCommandBufferAllocate(
    VulkanContext* _context,
    VkCommandPool _pool,
    b8 _isPrimary,
    VulkanCommandBuffer* _outCommandBuffer);

void VulkanCommandBufferFree(
    VulkanContext* _context,
    VkCommandPool _pool,
    VulkanCommandBuffer* _commandBuffer);

void VulkanCommandBufferBegin(
    VulkanCommandBuffer* _commandBuffer,
    b8 _isSingleUse,
    b8 _isRenderpassContinue,
    b8 _isSimultaneousUse);

void VulkanCommandBufferEnd(VulkanCommandBuffer* _commandBuffer);

void VulkanCommandBufferUpdateSubmitted(VulkanCommandBuffer* _commandBuffer);

void VulkanCommandBufferReset(VulkanCommandBuffer* _commandBuffer);

//allocates and begins recording to _outCommandBuffer
void VulkanCommandBufferAllocateAndBeginSingleUse(
    VulkanContext* _context,
    VkCommandPool _pool,
    VulkanCommandBuffer* _outCommandBuffer);

//Ends recording submits and waits for queue operation and frees provided command buffer.
void VulkanCommandBufferEndSingleUse(
    VulkanContext* _context,
    VkCommandPool _pool,
    VulkanCommandBuffer* _commandBuffer,
    VkQueue _queue);