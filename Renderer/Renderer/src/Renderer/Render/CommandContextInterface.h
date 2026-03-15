#pragma once

#include"Renderer/Core/Handles.h"

class ICommandContext {
public:
	virtual ~ICommandContext() = default;

    virtual void EndFrame() = 0;

    virtual void ClearRTV(TextureHandle handle, float clearColor[4]) = 0;
    virtual void ClearDSV(TextureHandle handle) = 0;

    virtual void SetViewport(uint32_t width, uint32_t height) = 0;

    virtual void SetVertexBuffer(BufferHandle buffer) = 0;
    virtual void SetIndexBuffer(BufferHandle buffer) = 0;

    virtual void UpdateBuffer(BufferHandle h, uint32_t byteWidth, void* data) = 0;

    virtual void SetBufferVS(BufferHandle buffer, uint32_t slot) = 0;
    virtual void SetBufferPS(BufferHandle buffer, uint32_t slot) = 0;

    virtual void SetTextureVS(TextureHandle buffer, uint32_t slot) = 0;
    virtual void SetTexturePS(TextureHandle buffer, uint32_t slot) = 0;

    virtual void SetPipeline(PipelineHandle pipeline) = 0;

    virtual void SetRenderTarget(RTVHandle rt) = 0;
    virtual void SetRenderTargets(size_t renderViewCount, Handle* renderViews, Handle depthView) = 0;

    virtual void Draw(uint32_t vertexCount, uint32_t startVertex) = 0;

    virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int baseVertexLocation) = 0;
};