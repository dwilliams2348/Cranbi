#pragma once

#include "VulkanTypes.inl"

/**
 * Returns the string representation of result.
 * @param _result The result to get the string for.
 * @param )getExtended Indicates whether to also return an extended result.
 * @returns The error code and/or extended error message in string form. Defaults to success for unknown result types.
 */
const char* VulkanResultString(VkResult _result, b8 _getExtended);

/**
 * Inticates if the passed result is a success or an error as defined by the Vulkan spec.
 * @returns True if success; otherwise false. Defaults to true for unknown result types.
 */
b8 VulkanResultIsSuccess(VkResult _result);