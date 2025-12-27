#pragma once

#include<d3d11.h>

#include<Renderer/Core/RenderItem.h>
#include<Renderer/Core/RenderContextInterface.h>
#include"ResourceProvider.h"

class D3D11RenderContext final : public zRender::IRenderContext {
public:
    D3D11RenderContext(ID3D11DeviceContext* context, IDXGISwapChain* swapChain, D3D11ResourceProvider* resourceProvider);

    void SetRenderTargetView(ID3D11RenderTargetView* renderTarget);
    void UpdateBuffer(Handle handle, UINT byteWidth, void* data) override;

    void BeginFrame() override;

    void SetViewport(int width, int height) override;
    void BindPipeline(const zRender::PipelineStateContainer& pipelineState) override;
    void BindBufferVS(uint32_t slot, Handle handle) override;
    void BindBufferPS(uint32_t slot, Handle handle) override;
    void BindTextureVS(uint32_t slot, Handle handle) override;
    void BindTexturePS(uint32_t slot, Handle handle) override;
    void DrawGeometryIndexed(Handle handle) override;

    void EndFrame() override;

private:
    IDXGISwapChain* swapChain = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3D11RenderTargetView* renderTarget = nullptr;
    ID3D11DepthStencilView* depthStencil = nullptr;
    D3D11ResourceProvider* resourceProvider = nullptr;
};