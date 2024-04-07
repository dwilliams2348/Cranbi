#include "VulkanRenderpass.h"

#include "core/CMemory.h"
#include "core/Logger.h"

void VulkanRenderpassCreate(VulkanContext* _context, 
    VulkanRenderpass* _outRenderpass, 
    f32 _x, f32 _y, f32 _w, f32 _h, 
    f32 _r, f32 _g, f32 _b, f32 _a, 
    f32 _depth, u32 _stencil) 
{
    LOG_INFO("Creating Vulkan render pass...");

    _outRenderpass->x = _x;
    _outRenderpass->y = _y;
    _outRenderpass->w = _w;
    _outRenderpass->h = _h;

    _outRenderpass->r = _r;
    _outRenderpass->g = _g;
    _outRenderpass->b = _b;
    _outRenderpass->a = _a;

    _outRenderpass->depth = _depth;
    _outRenderpass->stencil = _stencil;

    //create main subpass
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    //attachments TODO: make configurable
    const u32 attachmentDescriptionCount = 2;
    VkAttachmentDescription attachmentDescriptions[attachmentDescriptionCount];

    //color attachment
    VkAttachmentDescription colorAttachment;
    colorAttachment.format = _context->swapchain.imageFormat.format; //TODO: make configurable
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //do not expect a particular layout before starting render pass.
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //transitioned to after the render pass.
    colorAttachment.flags = 0;

    attachmentDescriptions[0] = colorAttachment;

    //color attachment reference
    VkAttachmentReference colorAttachmentReference;
    colorAttachmentReference.attachment = 0; //attachment description array index
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;

    //depth attachment, if there is one
    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = _context->device.depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    attachmentDescriptions[1] = depthAttachment;

    //depth attachment reference
    VkAttachmentReference depthAttachmentReference;
    depthAttachmentReference.attachment = 1; //attachment description array index
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    //TODO: other attachment types (input, resolve, preserve)

    //depth stencil data
    subpass.pDepthStencilAttachment = &depthAttachmentReference;

    //input from a shader
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = 0;

    //attachments used for multisampling color attachments
    subpass.pResolveAttachments = 0;

    //attachments not used in this subpass but used for the next pass
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = 0;

    //render pass dependenices. TODO: make this configurable
    VkSubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    //renderpass create
    VkRenderPassCreateInfo renderpassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    renderpassCreateInfo.attachmentCount = attachmentDescriptionCount;
    renderpassCreateInfo.pAttachments = attachmentDescriptions;
    renderpassCreateInfo.subpassCount = 1;
    renderpassCreateInfo.pSubpasses = &subpass;
    renderpassCreateInfo.dependencyCount = 1;
    renderpassCreateInfo.pDependencies = &dependency;
    renderpassCreateInfo.pNext = 0;
    renderpassCreateInfo.flags = 0;

    VK_CHECK(vkCreateRenderPass(_context->device.logicalDevice, &renderpassCreateInfo, _context->allocator, &_outRenderpass->handle));

    LOG_INFO("Vulkan render pass created successfully.");
}

void VulkanRenderpassDestroy(VulkanContext* _context, VulkanRenderpass* _renderpass) 
{
    if(_renderpass && _renderpass->handle)
    {
        vkDestroyRenderPass(_context->device.logicalDevice, _renderpass->handle, _context->allocator);
        _renderpass->handle = 0;
    }
}

void VulkanRenderpassBegin(VulkanCommandBuffer* _commandBuffer, VulkanRenderpass* _renderpass, VkFramebuffer _framebuffer) 
{
    VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    beginInfo.renderPass = _renderpass->handle;
    beginInfo.framebuffer = _framebuffer;
    beginInfo.renderArea.offset.x = _renderpass->x;
    beginInfo.renderArea.offset.y = _renderpass->y;
    beginInfo.renderArea.extent.width = _renderpass->w;
    beginInfo.renderArea.extent.height = _renderpass->h;

    VkClearValue clearValues[2];
    cZeroMemory(clearValues, sizeof(VkClearValue) * 2);
    clearValues[0].color.float32[0] = _renderpass->r;
    clearValues[0].color.float32[1] = _renderpass->g;
    clearValues[0].color.float32[2] = _renderpass->b;
    clearValues[0].color.float32[3] = _renderpass->a;
    clearValues[1].depthStencil.depth = _renderpass->depth;
    clearValues[1].depthStencil.stencil = _renderpass->stencil;

    beginInfo.clearValueCount = 2;
    beginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(_commandBuffer->handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    _commandBuffer->state = COMMAND_BUFFER_STATE_IN_RENDER_PASS;
}

void VulkanRenderpassEnd(VulkanCommandBuffer* _commandBuffer, VulkanRenderpass* _renderpass) 
{
    vkCmdEndRenderPass(_commandBuffer->handle);
    _commandBuffer->state = COMMAND_BUFFER_STATE_RECORDING;
}