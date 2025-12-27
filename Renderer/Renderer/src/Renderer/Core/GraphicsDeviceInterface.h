#pragma once

#include"Renderer/Core/PipelineStateContainer.h"

namespace zRender {
    class GraphicsDevice {
    public:
        virtual ~GraphicsDevice() = default;

        virtual void Initialize(void* windowHandle) = 0;
        virtual void Shutdown() = 0;
    };
}