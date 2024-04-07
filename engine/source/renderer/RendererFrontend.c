#include "RendererFrontend.h"
#include "RendererBackend.h"

#include "core/Logger.h"
#include "core/CMemory.h"

typedef struct RenderSystemState
{
    RendererBackend backend;
} RenderSystemState;

//backend render context
static RenderSystemState* pState;

b8 RendererSystemInitialize(u64* _memoryRequirement, void* _state, const char* _appName)
{
    *_memoryRequirement = sizeof(RenderSystemState);
    if(_state == 0)
        return true;
    
    pState = _state;

    //TODO: make configurable
    RendererBackendCreate(RENDERER_BACKEND_TYPE_VULKAN, &pState->backend);
    pState->backend.frameNumber = 0;

    if(!pState->backend.Initialize(&pState->backend, _appName))
    {
        LOG_FATAL("Renderer backend failed to initialize. Shutting down,");
        return false;
    }

    return true;
}

void RendererSystemShutdown(void* _state)
{
    if(pState)
        pState->backend.Shutdown(&pState->backend);

    pState = 0;
}

b8 RendererBeginFrame(f32 _deltaTime)
{
    if(!pState)
        return false;

    return pState->backend.BeginFrame(&pState->backend, _deltaTime);
}

b8 RendererEndFrame(f32 _deltaTime)
{
    if(!pState)
        return false;
    
    b8 result = pState->backend.EndFrame(&pState->backend, _deltaTime);
    pState->backend.frameNumber++;
    return result;
}

void RendererOnResize(u16 _width, u16 _height)
{
    if(pState)
        pState->backend.Resize(&pState->backend, _width, _height);
    else
        LOG_WARN("Renderer backend does not exist to accept resize: %i, %i", _width, _height);
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