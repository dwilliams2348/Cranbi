#pragma once

#include "RendererTypes.inl"

struct PlatformState;

b8 RendererBackendCreate(RendererBackendType _type, RendererBackend* _outBackend);
void RendererBackendDestroy(RendererBackend* _backend);