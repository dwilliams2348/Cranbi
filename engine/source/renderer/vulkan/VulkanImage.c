#include "VulkanImage.h"

#include "VulkanDevice.h"

#include "core/CMemory.h"
#include "core/Logger.h"

void VulkanImageCreate(
    VulkanContext* _context,
    VkImageType _imageType,
    u32 _width,
    u32 _height,
    VkFormat _format,
    VkImageTiling _tiling,
    VkImageUsageFlags _usage,
    VkMemoryPropertyFlags _memoryFlags,
    b32 _createView,
    VkImageAspectFlags _viewAspectFlags,
    VulkanImage* _outImage)
{
    //copy params
    _outImage->width = _width;
    _outImage->height = _height;

    //creation info
    VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = _width;
    imageCreateInfo.extent.height = _height;
    imageCreateInfo.extent.depth = 1;   //TODO: support configurable depth.
    imageCreateInfo.mipLevels = 4;      //TODO: support mip mapping
    imageCreateInfo.arrayLayers = 1;    //TODO: support number of layers in the image.
    imageCreateInfo.format = _format;
    imageCreateInfo.tiling = _tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = _usage;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;            //TODO: configurable sample count.
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;    //TODO: configurable sharing mode.

    VK_CHECK(vkCreateImage(_context->device.logicalDevice, &imageCreateInfo, _context->allocator, &_outImage->handle));

    //query memory requirements
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(_context->device.logicalDevice, _outImage->handle, &memoryRequirements);

    i32 memoryType = _context->FindMemoryIndex(memoryRequirements.memoryTypeBits, _memoryFlags);
    if(memoryType == -1)
        LOG_ERROR("Required memory type not found. Image not vaild.");

    //allocate memory
    VkMemoryAllocateInfo memoryAllocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryType;
    VK_CHECK(vkAllocateMemory(_context->device.logicalDevice, &memoryAllocateInfo, _context->allocator, &_outImage->memory));

    //Bind the memory
    VK_CHECK(vkBindImageMemory(_context->device.logicalDevice, _outImage->handle, _outImage->memory, 0)); //TODO: configurable memory offset.

    //create view
    if(_createView)
    {
        _outImage->view = 0;
        VulkanImageViewCreate(_context, _format, _outImage, _viewAspectFlags);
    }
}

void VulkanImageViewCreate(
    VulkanContext* _context,
    VkFormat _format,
    VulkanImage* _image,
    VkImageAspectFlags _aspectFlags)
{
    VkImageViewCreateInfo viewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    viewCreateInfo.image = _image->handle;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; //TODO: make configurable.
    viewCreateInfo.format = _format;
    viewCreateInfo.subresourceRange.aspectMask = _aspectFlags;

    //TODO: make configurable.
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(_context->device.logicalDevice, &viewCreateInfo, _context->allocator, &_image->view));
}

void VulkanImageDestroy(VulkanContext* _context, VulkanImage* _image)
{
    if(_image->view)
    {
        vkDestroyImageView(_context->device.logicalDevice, _image->view, _context->allocator);
        _image->view = 0;
    }
    if(_image->memory)
    {
        vkFreeMemory(_context->device.logicalDevice, _image->memory, _context->allocator);
        _image->memory = 0;
    }
    if(_image->handle)
    {
        vkDestroyImage(_context->device.logicalDevice, _image->handle, _context->allocator);
        _image->handle = 0;
    }
}