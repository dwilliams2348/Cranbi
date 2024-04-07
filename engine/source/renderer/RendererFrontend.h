#pragma once

#include "RendererTypes.inl"

b8 RendererSystemInitialize(u64* _memoryRequirement, void* _state, const char* _appName);
void RendererSystemShutdown(void* _state);

void RendererOnResize(u16 _width, u16 _height);

b8 RendererDrawFrame(RenderPacket* _packet);