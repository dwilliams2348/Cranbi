#include "VulkanShaderUtils.h"

#include "core/Logger.h"
#include "core/CMemory.h"
#include "core/CString.h"

#include "platform/Filesystem.h"

b8 CreateShaderModule(VulkanContext* _context, const char* _name, const char* _typeStr, VkShaderStageFlagBits _shaderStageFlag, u32 _stageIndex, VulkanShaderStage* _shaderStages) 
{
    //build file name
    char fileName[512];
    StringFormat(fileName, "assets/shaders/%s.%s.spv", _name, _typeStr);

    cZeroMemory(&_shaderStages[_stageIndex].createInfo, sizeof(VkShaderModuleCreateInfo));
    _shaderStages[_stageIndex].createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    //obtain file handle
    FileHandle handle;
    if(!FilesystemOpen(fileName, FILE_MODE_READ, true, &handle))
    {
        LOG_ERROR("Unable to read shader module: %s.", fileName);
        return false;
    }

    //read entire file as binary
    u64 size = 0;
    u8* fileBuffer = 0;
    if(!FilesystemReadAllBytes(&handle, &fileBuffer, &size))
    {
        LOG_ERROR("Unable to binary read shader module: %s.", fileName);
        return false;
    }

    _shaderStages[_stageIndex].createInfo.codeSize = size;
    _shaderStages[_stageIndex].createInfo.pCode = (u32*)fileBuffer;

    //close the file
    FilesystemClose(&handle);

    VK_CHECK(vkCreateShaderModule(_context->device.logicalDevice, &_shaderStages[_stageIndex].createInfo, _context->allocator, &_shaderStages[_stageIndex].handle));

    //shader stage info
    cZeroMemory(&_shaderStages[_stageIndex].shaderStageCreateInfo, sizeof(VkPipelineShaderStageCreateInfo));
    _shaderStages[_stageIndex].shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    _shaderStages[_stageIndex].shaderStageCreateInfo.stage = _shaderStageFlag;
    _shaderStages[_stageIndex].shaderStageCreateInfo.module = _shaderStages[_stageIndex].handle;
    _shaderStages[_stageIndex].shaderStageCreateInfo.pName = "main";

    if(fileBuffer)
    { 
        cFree(fileBuffer, sizeof(u8) * size, MEMORY_TAG_STRING);
        fileBuffer = 0;
    }

    return true;
}