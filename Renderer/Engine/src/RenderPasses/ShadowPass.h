#pragma once

#include"Renderer/Render/RenderPassInterface.h"
#include"Renderer/Render/RenderConstantBuffers.h"

class ShadowPass : public zRender::IRenderPass {
public:
	struct InitData {
		Handle depthSV;
		Handle debugRT;
		BufferHandle lightBufferHandle;
		BufferHandle objectBufferHandle;
		zRender::PipelineStateContainer pipeline;
	};

	struct LightBuffer {
		zRender::Light light[8];
	};

public:

	ShadowPass(const InitData& i);

	zRender::RenderPassDesc GetDesc() const override;
	void Execute(const zRender::RenderPassContext& ctx) override;

private:
	Handle depthSV;
	Handle debugRT;
	BufferHandle lightBufferHandle;
	BufferHandle objectBufferHandle;
	zRender::PipelineStateContainer pipeline;
};