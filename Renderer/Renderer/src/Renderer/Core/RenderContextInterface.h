#pragma once

#include"RenderItem.h"
#include"PipelineStateContainer.h"

namespace zRender {
    class IRenderContext {
    public:
        IRenderContext() {}

        virtual void UpdateBuffer(uint32_t handle, unsigned int byteWidth, void* data) = 0;
        virtual void SetViewport(int width, int height) = 0;

        virtual void BeginFrame() = 0;

        virtual void BindBufferVS(uint32_t slot, Handle handle) = 0;
        virtual void BindBufferPS(uint32_t slot, Handle handle) = 0;
        virtual void BindTextureVS(uint32_t slot, Handle handle) = 0;
        virtual void BindTexturePS(uint32_t slot, Handle handle) = 0;
        virtual void BindPipeline(const zRender::PipelineStateContainer& pipelineState) = 0;
        virtual void DrawGeometryIndexed(Handle handle) = 0;

        virtual void EndFrame() = 0;
    };
}