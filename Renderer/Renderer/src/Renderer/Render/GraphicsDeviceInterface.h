#pragma once

#include"Renderer/Render/PipelineStateContainer.h"

namespace zRender {
    class GraphicsDevice {
    public:
        virtual ~GraphicsDevice() = default;

        virtual void Initialize(void* windowHandle) = 0;
        virtual void Shutdown() = 0;
    };
}