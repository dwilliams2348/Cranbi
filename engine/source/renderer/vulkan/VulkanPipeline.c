#include "VulkanPipeline.h"
#include "VulkanUtils.h"

#include "core/CMemory.h"
#include "core/Logger.h"

#include "math/MathTypes.h"

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
    VulkanPipeline* _outPipeline)
{
    //viewport state
    VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewportState.viewportCount = 1;
    viewportState.pViewports = &_viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &_scissor;

    //rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    rasterizerCreateInfo.depthClampEnable = VK_FALSE;
    rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerCreateInfo.polygonMode = _isWireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    rasterizerCreateInfo.lineWidth = 1.f;
    rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizerCreateInfo.depthBiasConstantFactor = 0.f;
    rasterizerCreateInfo.depthBiasClamp = 0.f;
    rasterizerCreateInfo.depthBiasSlopeFactor = 0.f;

    //multisampling
    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
    multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisamplingCreateInfo.minSampleShading = 1.f;
    multisamplingCreateInfo.pSampleMask = 0;
    multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;

    //depth and stencil testing
    VkPipelineDepthStencilStateCreateInfo depthStencil = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
    cZeroMemory(&colorBlendAttachmentState, sizeof(VkPipelineColorBlendAttachmentState));
    colorBlendAttachmentState.blendEnable = VK_TRUE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    colorBlendAttachmentState.colorWriteMask =  VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;

    //dynamic state
    const u32 dynamicStateCount = 3;
    VkDynamicState dynamicStates[dynamicStateCount] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH};

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
    dynamicStateCreateInfo.pDynamicStates = dynamicStates;

    //vertex input
    VkVertexInputBindingDescription bindingDescription;
    bindingDescription.binding = 0; //binding index
    bindingDescription.stride = sizeof(Vertex3D);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //move to next data entry for each vertex.

    //attributes
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = _attributeCount;
    vertexInputInfo.pVertexAttributeDescriptions = _attributes;

    //input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    //pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

    //descriptor set layouts
    pipelineLayoutCreateInfo.setLayoutCount = _descriptorSetLayoutCount;
    pipelineLayoutCreateInfo.pSetLayouts = _descriptorSetLayouts;

    VK_CHECK(vkCreatePipelineLayout(_context->device.logicalDevice, &pipelineLayoutCreateInfo, _context->allocator, &_outPipeline->pipelineLayout));

    //pipeline create
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    pipelineCreateInfo.stageCount = _stageCount;
    pipelineCreateInfo.pStages = _stages;
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssembly;

    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencil;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.pTessellationState = 0;

    pipelineCreateInfo.layout = _outPipeline->pipelineLayout;

    pipelineCreateInfo.renderPass = _renderpass->handle;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    VkResult result = vkCreateGraphicsPipelines(
        _context->device.logicalDevice,
        VK_NULL_HANDLE,
        1,
        &pipelineCreateInfo,
        _context->allocator,
        &_outPipeline->handle);

    if(VulkanResultIsSuccess(result))
    {
        LOG_DEBUG("Graphics pipeline created.");
        return true;
    }

    LOG_ERROR("vkCreateGraphicsPipelines failed with %s", VulkanResultString(result, true));
    return false;
}

void VulkanPipelineDestroy(VulkanContext* _context, VulkanPipeline* _pipeline)
{
    //destroy pipeline
    if(_pipeline->handle)
    {
        vkDestroyPipeline(_context->device.logicalDevice, _pipeline->handle, _context->allocator);
        _pipeline->handle = 0;
    }

    //destroy layout
    if(_pipeline->pipelineLayout)
    {
        vkDestroyPipelineLayout(_context->device.logicalDevice, _pipeline->pipelineLayout, _context->allocator);
        _pipeline->handle = 0;
    }

    LOG_DEBUG("Graphics pipeline destroyed.");
}

void VulkanPipelineBind(VulkanCommandBuffer* _commandBuffer, VkPipelineBindPoint _bindPoint, VulkanPipeline* _pipeline)
{
    vkCmdBindPipeline(_commandBuffer->handle, _bindPoint, _pipeline->handle);
}