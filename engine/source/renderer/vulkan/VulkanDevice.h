#pragma once

#include "VulkanTypes.inl"

b8 VulkanDeviceCreate(VulkanContext* _context);

void VulkanDeviceDestroy(VulkanContext* _context);

void VulkanDeviceQuerySwapchainSupport(
    VkPhysicalDevice _physicalDevice,
    VkSurfaceKHR _surface,
    VulkanSwapchainSupportInfo* _outSupportInfo);

b8 VulkanDeviceDetectDepthFormat(VulkanDevice* _device);