#pragma once

#include "VulkanTypes.inl"

void VulkanFramebufferCreate(
    VulkanContext* _context,
    VulkanRenderpass* _renderpass,
    u32 _width, u32 _height,
    u32 _attachmentCount,
    VkImageView* _attachments,
    VulkanFramebuffer* _outFramebuffer);

void VulkanFramebufferDestroy(VulkanContext* _context, VulkanFramebuffer* _framebuffer);