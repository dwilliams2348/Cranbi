#pragma once

#include "renderer/vulkan/VulkanTypes.inl"
#include "renderer/RendererTypes.inl"

b8 VulkanObjectShaderCreate(VulkanContext* _context, VulkanObjectShader* _outShader);
void VulkanObjectShaderDestroy(VulkanContext* _context, struct VulkanObjectShader* _shader);

void VulkanObjectShaderUse(VulkanContext* _context, struct VulkanObjectShader* _shader);
