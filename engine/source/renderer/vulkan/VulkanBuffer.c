#include "VulkanBuffer.h"

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUtils.h"

#include "core/Logger.h"
#include "core/CMemory.h"

b8 VulkanBufferCreate(VulkanContext* _context, u64 _size, VkBufferUsageFlagBits _usage, u32 _memoryPropertyFlags, b8 _bindOnCreate, VulkanBuffer* _outBuffer) 
{
    cZeroMemory(_outBuffer, sizeof(VulkanBuffer));
    _outBuffer->totalSize = _size;
    _outBuffer->usage = _usage;
    _outBuffer->memoryPropertyFlags = _memoryPropertyFlags;

    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = _size;
    bufferInfo.usage = _usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // NOTE: only used in singular queue.

    VK_CHECK(vkCreateBuffer(_context->device.logicalDevice, &bufferInfo, _context->allocator, &_outBuffer->handle));

    //gather memory requirements
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(_context->device.logicalDevice, _outBuffer->handle, &requirements);
    _outBuffer->memoryIndex = _context->FindMemoryIndex(requirements.memoryTypeBits, _outBuffer->memoryPropertyFlags);
    if(_outBuffer->memoryIndex == -1)
    {
        LOG_ERROR("Unable to create vulkan buffer because the required memeory type index was not found.");
        return false;
    }

    //allocate memory info
    VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = (u32)_outBuffer->memoryIndex;

    //allocate memory
    VkResult result = vkAllocateMemory(_context->device.logicalDevice, &allocateInfo, _context->allocator, &_outBuffer->memory);

    if(result != VK_SUCCESS)
    {
        LOG_ERROR("Unable to create vulkan buffer becuase the required memory allocation failed. Error: %i", result);
        return false;
    }

    if(_bindOnCreate)
        VulkanBufferBind(_context, _outBuffer, 0);

    return true;
}

void VulkanBufferDestroy(VulkanContext* _context, VulkanBuffer* _buffer) 
{
    if(_buffer->memory)
    {
        vkFreeMemory(_context->device.logicalDevice, _buffer->memory, _context->allocator);
        _buffer->memory = 0;
    }
    if(_buffer->handle)
    {
        vkDestroyBuffer(_context->device.logicalDevice, _buffer->handle, _context->allocator);
        _buffer->handle = 0;
    }

    _buffer->totalSize = 0;
    _buffer->usage = 0;
    _buffer->isLocked = false;
}

b8 VulkanBufferResize(VulkanContext* _context, u64 _newSize, VulkanBuffer* _buffer, VkQueue _queue, VkCommandPool _pool) 
{
    //create new buffer
    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = _newSize;
    bufferInfo.usage = _buffer->usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // NOTE: only used in singular queue.

    VkBuffer newBuffer;
    VK_CHECK(vkCreateBuffer(_context->device.logicalDevice, &bufferInfo, _context->allocator, &newBuffer));

    //gather memory requirements
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(_context->device.logicalDevice, newBuffer, &requirements);

    //allocate memory info
    VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = (u32)_buffer->memoryIndex;

    //allocate memory
    VkDeviceMemory newMemory;
    VkResult result = vkAllocateMemory(_context->device.logicalDevice, &allocateInfo, _context->allocator, &newMemory);

    if(result != VK_SUCCESS)
    {
        LOG_ERROR("Unable to create vulkan buffer becuase the required memory allocation failed. Error: %i", result);
        return false;
    }

    //bind new buffers memory
    VK_CHECK(vkBindBufferMemory(_context->device.logicalDevice, newBuffer, newMemory, 0));

    //copy over the data
    VulkanBufferCopyTo(_context, _pool, 0, _queue, _buffer->handle, 0, newBuffer, 0, _buffer->totalSize);

    //make sure anything potentially using this is done using it
    vkDeviceWaitIdle(_context->device.logicalDevice);

    //destroy old buffer
    if(_buffer->memory)
    {
        vkFreeMemory(_context->device.logicalDevice, _buffer->memory, _context->allocator);
        _buffer->memory = 0;
    }
    if(_buffer->handle)
    {
        vkDestroyBuffer(_context->device.logicalDevice, _buffer->handle, _context->allocator);
        _buffer->handle = 0;
    }

    //set new properties
    _buffer->totalSize = _newSize;
    _buffer->memory = newMemory;
    _buffer->handle = newBuffer;

    return true;
}

void VulkanBufferBind(VulkanContext* _context, VulkanBuffer* _buffer, u64 _offset) 
{
    VK_CHECK(vkBindBufferMemory(_context->device.logicalDevice, _buffer->handle, _buffer->memory, _offset));
}

void* VulkanBufferLockMemory(VulkanContext* _context, VulkanBuffer* _buffer, u64 _offset, u64 _size, u32 _flags) 
{
    void* data;
    VK_CHECK(vkMapMemory(_context->device.logicalDevice, _buffer->memory, _offset, _size, _flags, &data));
    return data;
}

void VulkanBufferUnlockMemory(VulkanContext* _context, VulkanBuffer* _buffer) 
{
    vkUnmapMemory(_context->device.logicalDevice, _buffer->memory);
}

void VulkanBufferLoadData(VulkanContext* _context, VulkanBuffer* _buffer, u64 _offset, u64 _size, u32 _flags, const void* _data) 
{
    void* dataPtr;
    VK_CHECK(vkMapMemory(_context->device.logicalDevice, _buffer->memory, _offset, _size, _flags, &dataPtr));
    cCopyMemory(dataPtr, _data, _size);
    vkUnmapMemory(_context->device.logicalDevice, _buffer->memory);
}

void VulkanBufferCopyTo(VulkanContext* _context, VkCommandPool _pool, VkFence _fence, VkQueue _queue, VkBuffer _source, u64 _sourceOffset, VkBuffer _dest, u64 _destOffset, u64 _size) 
{
    vkQueueWaitIdle(_queue);

    //create a one time use command buffer
    VulkanCommandBuffer tempCommandBuffer;
    VulkanCommandBufferAllocateAndBeginSingleUse(_context, _pool, &tempCommandBuffer);

    //prepare the copy command and add it to the command buffer
    VkBufferCopy copyRegion;
    copyRegion.srcOffset = _sourceOffset;
    copyRegion.dstOffset = _destOffset;
    copyRegion.size = _size;

    vkCmdCopyBuffer(tempCommandBuffer.handle, _source, _dest, 1, &copyRegion);

    //submit the buffer for execution and wait to complete
    VulkanCommandBufferEndSingleUse(_context, _pool, &tempCommandBuffer, _queue);
}