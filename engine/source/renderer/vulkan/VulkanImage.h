#pragma once

#include "VulkanTypes.inl"

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
    VulkanImage* _outImage);

void VulkanImageViewCreate(
    VulkanContext* _context,
    VkFormat _format,
    VulkanImage* _image,
    VkImageAspectFlags _aspectFlags);

void VulkanImageDestroy(VulkanContext* _context, VulkanImage* _image);