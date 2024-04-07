#pragma once

#include "renderer/RendererBackend.h"

b8 VulkanRendererBackendInitialize(RendererBackend* _backend, const char* _appName, struct PlatformState* _platform);
void VulkanRendererBackendShutdown(RendererBackend* _backend);

void VulkanRendererBackendOnResize(RendererBackend* _backend, u16 _width, u16 _height);

b8 VulkanRendererBackendBeginFrame(RendererBackend* _backend, f32 _deltaTime);
b8 VulkanRendererBackendEndFrame(RendererBackend* _backend, f32 _deltaTime);