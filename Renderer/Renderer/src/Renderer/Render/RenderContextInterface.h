#pragma once

#include"Renderer/Render/RenderItem.h"
#include"Renderer/Render/PipelineStateContainer.h"

namespace zRender {
    class IRenderContext {
    public:
        virtual ~IRenderContext() {}

        virtual void ClearStatesAndResources() = 0;
        virtual void UpdateBuffer(BufferHandle handle, unsigned int byteWidth, void* data) = 0;
        virtual void SetViewport(int width, int height) = 0;

        virtual void BeginFrame() = 0;

        virtual void ClearRenderTarget(TextureHandle handle, float clearColor[4]) = 0;
        virtual void ClearDepthStencil(TextureHandle handle) = 0;
        virtual void BindMultiViews(size_t renderViewCount, Handle* renderViews, Handle depthView) = 0;
        virtual void BindBufferVS(uint32_t slot, BufferHandle handle) = 0;
        virtual void BindBufferPS(uint32_t slot, BufferHandle handle) = 0;
        virtual void BindTextureVS(uint32_t slot, TextureHandle handle) = 0;
        virtual void BindTexturePS(uint32_t slot, TextureHandle handle) = 0;
        virtual void BindPipeline(const PipelineStateContainer& pipelineState) = 0;
        virtual void DrawGeometryIndexed(MeshHandle handle, uint32_t subMeshIndex) = 0;
        virtual void DrawIndexed(uint64_t count) = 0;
        virtual void Draw(uint64_t count) = 0;

        virtual void EndFrame() = 0;
    };
}