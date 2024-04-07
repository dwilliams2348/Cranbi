#pragma once

#include "Defines.h"

typedef enum RendererBackendType
{
    RENDERER_BACKEND_TYPE_VULKAN,
    RENDERER_BACKEND_TYPE_OPENGL,
    RENDERER_BACKEND_TYPE_DIRECTX
} RendererBackendType;

typedef struct RendererBackend
{
    u64 frameNumber;

    b8 (*Initialize)(struct RendererBackend* _backend, const char* _appName);
    void (*Shutdown)(struct RendererBackend* _backend);

    void (*Resize)(struct RendererBackend* _backend, u16 _width, u16 _height);

    b8 (*BeginFrame)(struct RendererBackend* _backend, f32 _deltaTime);
    b8 (*EndFrame)(struct RendererBackend* _backend, f32 _deltaTime);
} RendererBackend;

typedef struct RenderPacket
{
    f32 deltaTime;
} RenderPacket;