#pragma once

#include<d3d11.h>

#include"Renderer/Render/RenderItem.h"
#include"Renderer/Render/RenderContextInterface.h"
#include"Renderer/D3D11/ResourceProvider.h"

namespace zRender {
    class D3D11RenderContext final : public IRenderContext {
    public:
        D3D11RenderContext(ID3D11DeviceContext* context, IDXGISwapChain* swapChain, D3D11ResourceProvider* resourceProvider);

        void UpdateBuffer(BufferHandle handle, UINT byteWidth, void* data) override;
        void ClearStatesAndResources() override;

        void BeginFrame() override;

        void ClearRenderTarget(TextureHandle handle, float clearColor[4]) override;
        void ClearDepthStencil(TextureHandle handle) override;
        void BindMultiViews(size_t renderViewCount, Handle* renderViews, Handle depthView) override;
        void SetViewport(int width, int height) override;
        void BindPipeline(const PipelineStateContainer& pipelineState) override;
        void BindBufferVS(uint32_t slot, BufferHandle handle) override;
        void BindBufferPS(uint32_t slot, BufferHandle handle) override;
        void BindTextureVS(uint32_t slot, TextureHandle handle) override;
        void BindTexturePS(uint32_t slot, TextureHandle handle) override;
        void DrawGeometryIndexed(MeshHandle handle, uint32_t subMeshIndex = 0) override;
        void Draw(uint64_t count) override;
        void DrawIndexed(uint64_t count) override;

        void EndFrame() override;

    private:
        IDXGISwapChain* swapChain = nullptr;
        ID3D11DeviceContext* context = nullptr;
        D3D11ResourceProvider* resourceProvider = nullptr;
    };
}