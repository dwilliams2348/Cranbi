#pragma once

#include "VulkanTypes.inl"

b8 CreateShaderModule(
    VulkanContext* _context,
    const char* _name,
    const char* _typeStr,
    VkShaderStageFlagBits _shaderStageFlag,
    u32 _stageIndex,
    VulkanShaderStage* _shaderStages);