#include "VulkanFramebuffer.h"

#include "core/CMemory.h"

void VulkanFramebufferCreate(VulkanContext* _context, VulkanRenderpass* _renderpass, u32 _width, u32 _height, u32 _attachmentCount, VkImageView* _attachments, VulkanFramebuffer* _outFramebuffer) 
{
    //take a copy of the attachments, renderpass and attachment count
    _outFramebuffer->attachments = cAllocate(sizeof(VkImageView) * _attachmentCount, MEMORY_TAG_RENDERER);
    for(u32 i = 0; i < _attachmentCount; ++i)
        _outFramebuffer->attachments[i] = _attachments[i];
    
    _outFramebuffer->renderpass = _renderpass;
    _outFramebuffer->attachmentCount = _attachmentCount;

    //creation info
    VkFramebufferCreateInfo framebufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    framebufferCreateInfo.renderPass = _renderpass->handle;
    framebufferCreateInfo.attachmentCount = _attachmentCount;
    framebufferCreateInfo.pAttachments = _outFramebuffer->attachments;
    framebufferCreateInfo.width = _width;
    framebufferCreateInfo.height = _height;
    framebufferCreateInfo.layers = 1;

    VK_CHECK(vkCreateFramebuffer(_context->device.logicalDevice, &framebufferCreateInfo, _context->allocator, &_outFramebuffer->handle));
}

void VulkanFramebufferDestroy(VulkanContext* _context, VulkanFramebuffer* _framebuffer) 
{
    vkDestroyFramebuffer(_context->device.logicalDevice, _framebuffer->handle, _context->allocator);
    if(_framebuffer->attachments)
    {
        cFree(_framebuffer->attachments, sizeof(VkImageView) * _framebuffer->attachmentCount, MEMORY_TAG_RENDERER);
        _framebuffer->attachments = 0;
    }

    _framebuffer->handle = 0;
    _framebuffer->attachmentCount = 0;
    _framebuffer->renderpass = 0;
}
