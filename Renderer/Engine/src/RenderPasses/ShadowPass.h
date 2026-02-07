#pragma once

#include"Renderer/Render/RenderPassInterface.h"

class ShadowPass : public zRender::IRenderPass {
public:
	struct InitData {
		Handle depthSV;
		BufferHandle frameBufferHandle;
		BufferHandle objectBufferHandle;
		zRender::PipelineStateContainer pipeline;
	};

public:

	ShadowPass(const InitData& i);

	zRender::RenderPassDesc GetDesc() const override;
	void Execute(const zRender::RenderPassContext& ctx) override;

private:
	Handle depthSV;
	BufferHandle frameBufferHandle;
	BufferHandle objectBufferHandle;
	zRender::PipelineStateContainer pipeline;
};