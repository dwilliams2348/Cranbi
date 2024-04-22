#pragma once

#include "VulkanTypes.inl"

b8 VulkanBufferCreate(
    VulkanContext* _context,
    u64 _size,
    VkBufferUsageFlagBits _usage,
    u32 _memoryPropertyFlags,
    b8 _bindOnCreate,
    VulkanBuffer* _outBuffer);

void VulkanBufferDestroy(VulkanContext* _context, VulkanBuffer* _buffer);

b8 VulkanBufferResize(VulkanContext* _context, u64 _newSize, VulkanBuffer* _buffer, VkQueue _queue, VkCommandPool _pool);

void VulkanBufferBind(VulkanContext* _context, VulkanBuffer* _buffer, u64 _offset);

void* VulkanBufferLockMemory(VulkanContext* _context, VulkanBuffer* _buffer, u64 _offset, u64 _size, u32 _flags);
void VulkanBufferUnlockMemory(VulkanContext* _context, VulkanBuffer* _buffer);

void VulkanBufferLoadData(VulkanContext* _context, VulkanBuffer* _buffer, u64 _offset, u64 _size, u32 _flags, const void* _data);

void VulkanBufferCopyTo(
    VulkanContext* _context,
    VkCommandPool _pool,
    VkFence _fence,
    VkQueue _queue,
    VkBuffer _source,
    u64 _sourceOffset,
    VkBuffer _dest,
    u64 _destOffset,
    u64 _size);