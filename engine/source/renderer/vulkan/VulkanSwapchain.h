#pragma once

#include "VulkanTypes.inl"

void VulkanSwapchainCreate(
    VulkanContext* _context,
    u32 _width,
    u32 _height,
    VulkanSwapchain* _outSwapchain);

void VulkanSwapchainRecreate(
    VulkanContext* _context,
    u32 _width,
    u32 _height,
    VulkanSwapchain* _swapchain);

void VulkanSwapchainDestroy(VulkanContext* _context, VulkanSwapchain* _swapchain);

b8 VulkanSwapchainAquireNextImageIndex(
    VulkanContext* _context,
    VulkanSwapchain* _swapchain,
    u64 _timeoutNS,
    VkSemaphore _imageAvailableSemaphore,
    VkFence _fence,
    u32* _outImageIndex);

void VulkanSwapchainPresent(
    VulkanContext* _context,
    VulkanSwapchain* _swapchain,
    VkQueue _graphicsQueue,
    VkQueue _presentQueue,
    VkSemaphore _renderCompleteSemaphore,
    u32 _presentImageIndex);