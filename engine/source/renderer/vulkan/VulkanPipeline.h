#pragma once

#include "VulkanTypes.inl"

b8 VulkanGraphicsPipelineCreate(
    VulkanContext* _context,
    VulkanRenderpass* _renderpass,
    u32 _attributeCount,
    VkVertexInputAttributeDescription* _attributes,
    u32 _descriptorSetLayoutCount,
    VkDescriptorSetLayout* _descriptorSetLayouts,
    u32 _stageCount,
    VkPipelineShaderStageCreateInfo* _stages,
    VkViewport _viewport,
    VkRect2D _scissor,
    b8 _isWireframe,
    VulkanPipeline* _outPipeline);

void VulkanPipelineDestroy(VulkanContext* _context, VulkanPipeline* _pipeline);

void VulkanPipelineBind(VulkanCommandBuffer* _commandBuffer, VkPipelineBindPoint _bindPoint, VulkanPipeline* _pipeline);