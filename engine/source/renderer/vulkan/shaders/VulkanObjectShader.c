#include "VulkanObjectShader.h"

#include "core/Logger.h"

#include "renderer/vulkan/VulkanShaderUtils.h"

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

    //descriptors
    

    return true;
}

void VulkanObjectShaderDestroy(VulkanContext* _context, VulkanObjectShader* _shader) 
{

}

void VulkanObjectShaderUse(VulkanContext* _context, VulkanObjectShader* _shader) 
{

}