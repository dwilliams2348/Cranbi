#pragma once

#include "VulkanTypes.inl"

void VulkanRenderpassCreate(
    VulkanContext* _context,
    VulkanRenderpass* _outRenderpass,
    f32 _x, f32 _y, f32 _w, f32 _h,
    f32 _r, f32 _g, f32 _b, f32 _a,
    f32 _depth, u32 _stencil);

void VulkanRenderpassDestroy(VulkanContext* _context, VulkanRenderpass* _renderpass);

void VulkanRenderpassBegin(
    VulkanCommandBuffer* _commandBuffer,
    VulkanRenderpass* _renderpass,
    VkFramebuffer _framebuffer);

void VulkanRenderpassEnd(VulkanCommandBuffer* _commandBuffer, VulkanRenderpass* _renderpass);