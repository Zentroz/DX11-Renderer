#pragma once

#include"Renderer/Render/PipelineStateContainer.h"
#include"Renderer/Render/ResourceProviderInterface.h"
#include"Renderer/Render/CommandContextInterface.h"

namespace zRender {
    class GraphicsDevice {
    public:
        virtual ~GraphicsDevice() = default;

        virtual void Initialize(void* windowHandle) = 0;
        virtual void Release() = 0;

        virtual IRenderResourceProvider* CreateResourceProvider() = 0;
        virtual ICommandContext* CreateCommandContext() = 0;

        virtual void EndFrame() = 0;
    };
}