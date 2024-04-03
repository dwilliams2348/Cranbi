#pragma once

#include "RendererTypes.inl"

struct StaticMeshData;
struct PlatformState;

b8 RendererInitialize(const char* _appName, struct PlatformState* _platform);
void RendererShutdown();

void RendererOnResize(u16 _width, u16 _height);

b8 RendererDrawFrame(RenderPacket* _packet);