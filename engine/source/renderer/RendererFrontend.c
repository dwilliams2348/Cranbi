#include "RendererFrontend.h"
#include "RendererBackend.h"

#include "core/Logger.h"
#include "core/CMemory.h"

//backend render context
static RendererBackend* backend = 0;

b8 RendererInitialize(const char* _appName, struct PlatformState* _platform)
{
    backend = cAllocate(sizeof(RendererBackend), MEMORY_TAG_RENDERER);

    //TODO: make configurable
    RendererBackendCreate(RENDERER_BACKEND_TYPE_VULKAN, _platform, backend);
    backend->frameNumber = 0;

    if(!backend->Initialize(backend, _appName, _platform))
    {
        LOG_FATAL("Renderer backend failed to initialize. Shutting down,");
        return false;
    }

    return true;
}

void RendererShutdown()
{
    backend->Shutdown(backend);
    cFree(backend, sizeof(RendererBackend), MEMORY_TAG_RENDERER);
}

void RendererOnResize(u16 _width, u16 _height)
{
    if(backend)
        backend->Resize(backend, _width, _height);
    else
        LOG_WARN("Renderer backend does not exist to accept resize: %i, %i", _width, _height);
}

b8 RendererBeginFrame(f32 _deltaTime)
{
    return backend->BeginFrame(backend, _deltaTime);
}

b8 RendererEndFrame(f32 _deltaTime)
{
    b8 result = backend->EndFrame(backend, _deltaTime);
    backend->frameNumber++;
    return result;
}

b8 RendererDrawFrame(RenderPacket* _packet)
{
    //if the begin frame returned successfull mid frame ops can continue
    if(RendererBeginFrame(_packet->deltaTime))
    {

        //end frame, if this is fails it is likely unrecoverable
        b8 result = RendererEndFrame(_packet->deltaTime);
        if(!result)
        {
            LOG_ERROR("RendererEndFrame failed, application shutting down...");
            return false;
        }
    }

    return true;
}