#pragma once

#include<d3d11.h>
#include<wrl.h>
#include"Renderer/D3D11/ResourceStorage.h"
#include"Renderer/Render/CommandContextInterface.h"

class D3D11CommandContext : public ICommandContext {
public:
    D3D11CommandContext(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : context(context), storage(nullptr) {}
    ~D3D11CommandContext() = default;

    void EndFrame() override;

    void SetStorage(D3D11ResourceStorage* storage);

    void ClearRTV(TextureHandle handle, float clearColor[4]) override;
    void ClearDSV(TextureHandle handle) override;

    void SetViewport(uint32_t width, uint32_t height) override;

    void SetVertexBuffer(BufferHandle buffer) override;
    void SetIndexBuffer(BufferHandle buffer) override;

    void UpdateBuffer(BufferHandle h, uint32_t byteWidth, void* data) override;

    void SetBufferVS(BufferHandle buffer, uint32_t slot) override;
    void SetBufferPS(BufferHandle buffer, uint32_t slot) override;

    void SetTextureVS(TextureHandle buffer, uint32_t slot) override;
    void SetTexturePS(TextureHandle buffer, uint32_t slot) override;

    void SetPipeline(PipelineHandle pipeline) override;

    void SetRenderTarget(RTVHandle rt) override;
    void SetRenderTargets(size_t renderViewCount, Handle* renderViews, Handle depthView) override;

    void Draw(uint32_t vertexCount, uint32_t startVertex) override;

    void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int baseVertexLocation) override;

private:
    D3D11ResourceStorage* storage;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
};