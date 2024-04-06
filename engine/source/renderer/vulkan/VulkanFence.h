#pragma once

#include "VulkanTypes.inl"

void VulkanFenceCreate(
    VulkanContext* _context,
    b8 _createSignaled,
    VulkanFence* _outFence);

void VulkanFenceDestroy(VulkanContext* _context, VulkanFence* _fence);

b8 VulkanFenceWait(VulkanContext* _context, VulkanFence* _fence, u64 _timeoutNS);

void VulkanFenceReset(VulkanContext* _context, VulkanFence* _fence);