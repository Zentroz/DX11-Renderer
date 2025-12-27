#pragma once

#include<d3d11.h>

#include"Renderer/Render/RenderItem.h"
#include"Renderer/Render/RenderContextInterface.h"
#include"Renderer/D3D11/ResourceProvider.h"

namespace zRender {
    class D3D11RenderContext final : public IRenderContext {
    public:
        D3D11RenderContext(ID3D11DeviceContext* context, IDXGISwapChain* swapChain, D3D11ResourceProvider* resourceProvider);

        void SetRenderTargetView(ID3D11RenderTargetView* renderTarget);
        void UpdateBuffer(BufferHandle handle, UINT byteWidth, void* data) override;

        void BeginFrame() override;

        void SetViewport(int width, int height) override;
        void BindPipeline(const PipelineStateContainer& pipelineState) override;
        void BindBufferVS(uint32_t slot, BufferHandle handle) override;
        void BindBufferPS(uint32_t slot, BufferHandle handle) override;
        void BindTextureVS(uint32_t slot, TextureHandle handle) override;
        void BindTexturePS(uint32_t slot, TextureHandle handle) override;
        void DrawGeometryIndexed(MeshHandle handle) override;

        void EndFrame() override;

    private:
        IDXGISwapChain* swapChain = nullptr;
        ID3D11DeviceContext* context = nullptr;
        ID3D11RenderTargetView* renderTarget = nullptr;
        ID3D11DepthStencilView* depthStencil = nullptr;
        D3D11ResourceProvider* resourceProvider = nullptr;
    };
}