#include "RendererBackend.h"

#include "vulkan/VulkanBackend.h"

b8 RendererBackendCreate(RendererBackendType _type, struct PlatformState* _platform, RendererBackend* _outBackend)
{
    _outBackend->platform = _platform;

    if(_type == RENDERER_BACKEND_TYPE_VULKAN)
    {
        _outBackend->Initialize = VulkanRendererBackendInitialize;
        _outBackend->Shutdown = VulkanRendererBackendShutdown;
        _outBackend->BeginFrame = VulkanRendererBackendBeginFrame;
        _outBackend->EndFrame = VulkanRendererBackendEndFrame;
        _outBackend->Resize = VulkanRendererBackendOnResize;

        return TRUE;
    }

    return FALSE;
}

void RendererBackendDestroy(RendererBackend* _backend)
{
    _backend->Initialize = 0;
    _backend->Shutdown = 0;
    _backend->BeginFrame = 0;
    _backend->EndFrame = 0;
    _backend->Resize = 0;
}