#include "VulkanObjectShader.h"

#include "core/Logger.h"
#include "core/CMemory.h"

#include "math/MathTypes.h"

#include "renderer/vulkan/VulkanShaderUtils.h"
#include "renderer/vulkan/VulkanPipeline.h"

#define BUILTIN_SHADER_NAME_OBJECT "Builtin.ObjectShader"

b8 VulkanObjectShaderCreate(VulkanContext* _context, VulkanObjectShader* _outShader) 
{
    //shader module init per stage
    char stageTypeStrs[OBJECT_SHADER_STAGE_COUNT][5] = { "vert", "frag" };
    VkShaderStageFlagBits stageTypes[OBJECT_SHADER_STAGE_COUNT] = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };

    //create shader modules
    for(u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i)
    {
        if(!CreateShaderModule(_context, BUILTIN_SHADER_NAME_OBJECT, stageTypeStrs[i], stageTypes[i], i, _outShader->stages))
        {
            LOG_ERROR("Unable to create %s shader module for '%s'.", stageTypeStrs[i], BUILTIN_SHADER_NAME_OBJECT);
            return false;
        }
    }

    //TODO: descriptors

    //pipeline creation
    VkViewport viewport;
    viewport.x = 0.f;
    viewport.y = (f32)_context->framebufferHeight;
    viewport.width = (f32)_context->framebufferWidth;
    viewport.height = -(f32)_context->framebufferHeight;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    //scissor
    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = _context->framebufferWidth;
    scissor.extent.height = _context->framebufferHeight;

    //attributes
    u32 offset = 0;
    const i32 attributeCount = 1;
    VkVertexInputAttributeDescription attributeDescriptions[attributeCount];
    //position
    VkFormat formats[attributeCount] = {
        VK_FORMAT_R32G32B32_SFLOAT
    };

    u64 sizes[attributeCount] = {
        sizeof(vec3)
    };

    for(u32 i = 0; i < attributeCount; ++i)
    {
        attributeDescriptions[i].binding = 0; //binding index, should match bindin desc
        attributeDescriptions[i].location = i; //attrib location
        attributeDescriptions[i].format = formats[i];
        attributeDescriptions[i].offset = offset;
        offset += sizes[i];
    }

    //TODO: descriptor set layouts

    //stages
    //NOTE: should match the number of shader->stages
    VkPipelineShaderStageCreateInfo stageCreateInfos[OBJECT_SHADER_STAGE_COUNT];
    cZeroMemory(stageCreateInfos, sizeof(stageCreateInfos));
    for(u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i)
    {
        stageCreateInfos[i].sType = _outShader->stages[i].shaderStageCreateInfo.sType;
        stageCreateInfos[i] = _outShader->stages[i].shaderStageCreateInfo;
    }

    //create pipeline
    if(!VulkanGraphicsPipelineCreate(
        _context,
        &_context->mainRenderpass,
        attributeCount,
        attributeDescriptions,
        0,
        0,
        OBJECT_SHADER_STAGE_COUNT,
        stageCreateInfos,
        viewport,
        scissor,
        false, //isWireframe
        &_outShader->pipeline))
    {
        LOG_ERROR("Failed to load grpahics pipeline for object shader.");
        return false;
    }

    return true;
}

void VulkanObjectShaderDestroy(VulkanContext* _context, VulkanObjectShader* _shader) 
{
    //destroy graphics pipeline
    VulkanPipelineDestroy(_context, &_shader->pipeline);

    //destroy shader module
    for(u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i)
    {
        vkDestroyShaderModule(_context->device.logicalDevice, _shader->stages[i].handle, _context->allocator);
        _shader->stages[i].handle = 0;
    }
}

void VulkanObjectShaderUse(VulkanContext* _context, VulkanObjectShader* _shader) 
{

}