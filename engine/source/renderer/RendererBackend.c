#include "RendererBackend.h"

#include "vulkan/VulkanBackend.h"

b8 RendererBackendCreate(RendererBackendType _type, RendererBackend* _outBackend)
{
    if(_type == RENDERER_BACKEND_TYPE_VULKAN)
    {
        _outBackend->Initialize = VulkanRendererBackendInitialize;
        _outBackend->Shutdown = VulkanRendererBackendShutdown;
        _outBackend->BeginFrame = VulkanRendererBackendBeginFrame;
        _outBackend->EndFrame = VulkanRendererBackendEndFrame;
        _outBackend->Resize = VulkanRendererBackendOnResize;

        return true;
    }

    return false;
}

void RendererBackendDestroy(RendererBackend* _backend)
{
    _backend->Initialize = 0;
    _backend->Shutdown = 0;
    _backend->BeginFrame = 0;
    _backend->EndFrame = 0;
    _backend->Resize = 0;
}